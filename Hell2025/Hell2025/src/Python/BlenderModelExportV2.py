import bpy, struct, time, os, mathutils, numpy as np

# Axis conversion from Blender Z-up to engine Y-up
def make_conversion_matrices():
    conv3 = mathutils.Matrix(((1,0,0), (0,0,1), (0,-1,0)))
    conv4 = mathutils.Matrix(((1,0,0,0), (0,0,1,0), (0,-1,0,0), (0,0,0,1)))
    return conv3, conv4, conv4.inverted()

class AABB:
    def __init__(self):
        self.min = np.array([np.inf, np.inf, np.inf])
        self.max = np.array([-np.inf, -np.inf, -np.inf])
    def update(self, point):
        self.min = np.minimum(self.min, point)
        self.max = np.maximum(self.max, point)
    def to_tuple(self):
        return tuple(self.min), tuple(self.max)

class ModelExporter:
    def __init__(self, filepath, rounding=6):
        self.filepath = filepath
        self.rounding = rounding
        # Only visible mesh objects
        self.all_meshes = [obj for obj in bpy.context.scene.objects if obj.type=='MESH' and obj.visible_get()]
        self.sorted_meshes = self._sort_meshes_by_parent()
        self.mesh_index_map = {obj.name:i for i,obj in enumerate(self.sorted_meshes)}
        self.conv3, self.conv4, self.conv4_inv = make_conversion_matrices()

    def _sort_meshes_by_parent(self):
        """Sorts the mesh objects so that no child appears before its parent."""
        dependency_graph = bpy.context.evaluated_depsgraph_get()
        parent_map = {obj.name: obj.parent for obj in self.all_meshes}
        sorted_meshes = []
        processed = set()

        def add_mesh(obj):
            if obj.name not in processed:
                parent = parent_map.get(obj.name)
                if parent and parent.type == 'MESH':
                    add_mesh(parent)
                sorted_meshes.append(obj)
                processed.add(obj.name)

        for obj in self.all_meshes:
            add_mesh(obj)

        return sorted_meshes

    def triangulate(self, obj):
        deps = bpy.context.evaluated_depsgraph_get()
        mesh = obj.evaluated_get(deps).to_mesh()
        mesh.calc_loop_triangles()
        return mesh

    def compute_aabbs(self):
        self.aabbs = {}
        for obj in self.sorted_meshes:
            mesh = self.triangulate(obj)
            aabb = AABB()
            for v in mesh.vertices:
                p = self.conv3 @ v.co
                aabb.update(np.array([p.x, p.y, p.z]))
            self.aabbs[obj.name] = aabb
            obj.to_mesh_clear()

    def export(self):
        start = time.time()
        self.compute_aabbs()

        # Compute scene AABB
        scene_bb = AABB()
        for bb in self.aabbs.values():
            scene_bb.update(bb.min)
            scene_bb.update(bb.max)

        with open(self.filepath, 'wb') as f:
            # Model header
            f.write(b"HELL_MODEL".ljust(32, b'\0'))
            f.write(struct.pack('<I', 2))
            f.write(struct.pack('<I', len(self.sorted_meshes)))
            f.write(struct.pack('<Q', int(time.time())))
            mn, mx = scene_bb.to_tuple()
            f.write(struct.pack('<3f', *mn))
            f.write(struct.pack('<3f', *mx))

            # Per-mesh data
            for obj in self.sorted_meshes:
                mesh = self.triangulate(obj)
                if mesh.uv_layers.active:
                    mesh.calc_tangents()

                # Build unique vertex list and index buffer
                vert_map = {}
                verts = []
                indices = []
                for tri in mesh.loop_triangles:
                    for li in tri.loops:
                        loop = mesh.loops[li]
                        v = mesh.vertices[loop.vertex_index]
                        # Mesh-space position, normal, tangent
                        p3 = self.conv3 @ v.co
                        n3 = (self.conv3 @ loop.normal).normalized()
                        t3 = (self.conv3 @ loop.tangent).normalized() if mesh.uv_layers.active else mathutils.Vector((0,0,0))
                        uv = mesh.uv_layers.active.data[li].uv if mesh.uv_layers.active else (0.0,0.0)
                        uv2 = (uv.x, 1-uv.y)
                        key = (
                            round(p3.x, self.rounding), round(p3.y, self.rounding), round(p3.z, self.rounding),
                            round(uv2[0], self.rounding), round(uv2[1], self.rounding)
                        )
                        bucket = vert_map.setdefault(key, [])
                        found = False
                        for idx, sum_n, sum_t, cnt in bucket:
                            if (sum_n/cnt).dot(n3) >= 0.95:
                                bucket[bucket.index((idx,sum_n,sum_t,cnt))] = (idx, sum_n+n3, sum_t+t3, cnt+1)
                                indices.append(idx)
                                found = True
                                break
                        if not found:
                            idx = len(verts)
                            bucket.append((idx, n3.copy(), t3.copy(), 1))
                            verts.append([p3.x, p3.y, p3.z, n3.x, n3.y, n3.z, uv2[0], uv2[1], t3.x, t3.y, t3.z])
                            indices.append(idx)

                # Finalize normals & tangents
                for bucket in vert_map.values():
                    for idx, sum_n, sum_t, cnt in bucket:
                        avg_n = (sum_n/cnt).normalized()
                        avg_t = (sum_t/cnt).normalized()
                        verts[idx][3:6] = [avg_n.x, avg_n.y, avg_n.z]
                        verts[idx][8:11] = [avg_t.x, avg_t.y, avg_t.z]

                # Mesh header
                bb = self.aabbs[obj.name]
                mn, mx = bb.to_tuple()
                f.write(b"HELL_MESH".ljust(32, b'\0'))
                name = obj.name.encode('utf-8')[:256]
                f.write(name.ljust(256, b'\0'))
                f.write(struct.pack('<I', len(verts)))
                f.write(struct.pack('<I', len(indices)))
                parent = obj.parent.name if obj.parent else None
                pi = self.mesh_index_map.get(parent, -1)
                f.write(struct.pack('<i', pi))
                f.write(struct.pack('<3f', *mn))
                f.write(struct.pack('<3f', *mx))

                # True parent-relative local transform via world matrices
                W = self.conv4 @ obj.matrix_world @ self.conv4_inv
                if obj.parent:
                    P = self.conv4 @ obj.parent.matrix_world @ self.conv4_inv
                    local4 = P.inverted() @ W
                else:
                    local4 = W
                inv4 = local4.inverted()
                # Flatten row-major for GLM
                flat_l = [local4[r][c] for c in range(4) for r in range(4)]
                flat_i = [inv4[r][c] for c in range(4) for r in range(4)]
                f.write(struct.pack('<16f', *flat_l))
                f.write(struct.pack('<16f', *flat_i))

                # Vertex & index buffers
                for v in verts:
                    f.write(struct.pack('<3f3f2f3f', *v))
                for i in indices:
                    f.write(struct.pack('<I', i))

                obj.to_mesh_clear()

        print(f"Export took {time.time()-start:.2f}s")

# Usage:
exporter = ModelExporter(
    'C:/Hell2025/Hell2025/Hell2025/res/models/Piano.model',
    rounding=6
)
exporter.export()