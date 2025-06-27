#include "Bvh.h"
#include "Renderer/Renderer.h"

namespace Bvh::Gpu {

    void RenderMesh(uint64_t bvhId, glm::vec4 color, glm::mat4 worldTransform) {
        if (!MeshBvhExists(bvhId)) return;
            
        MeshBvh* meshBvh = GetMeshBvhById(bvhId);
        for (int i = 0; i < meshBvh->m_triangleData.size(); i += 12) { // 12 floats per triangle
            glm::vec3 p0 = glm::vec3(meshBvh->m_triangleData[i], meshBvh->m_triangleData[i + 1], meshBvh->m_triangleData[i + 2]);
            glm::vec3 e1 = glm::vec3(meshBvh->m_triangleData[i + 3], meshBvh->m_triangleData[i + 4], meshBvh->m_triangleData[i + 5]);
            glm::vec3 e2 = glm::vec3(meshBvh->m_triangleData[i + 6], meshBvh->m_triangleData[i + 7], meshBvh->m_triangleData[i + 8]);
            glm::vec3 p1 = p0 - e1;
            glm::vec3 p2 = p0 + e2;
            p0 = worldTransform * glm::vec4(p0, 1.0f);
            p1 = worldTransform * glm::vec4(p1, 1.0f);
            p2 = worldTransform * glm::vec4(p2, 1.0f);
            Renderer::DrawLine(p0, p1, color);
            Renderer::DrawLine(p1, p2, color);
            Renderer::DrawLine(p2, p0, color);
        }
    }

    void RenderSceneBvh(uint64_t bvhId, glm::vec4 color) {
        if (!SceneBvhExists(bvhId)) return;

        SceneBvh* sceneMeshBvh = GetSceneBvhById(bvhId);
        glm::mat4 worldTransform = glm::mat4(1.0f);


        //std::cout << "node count: " << sceneMeshBvh->m_nodes.size() << "\n";

        for (BvhNode& node : sceneMeshBvh->m_nodes) {
            AABB aabb(node.boundsMin, node.boundsMax);
            Renderer::DrawAABB(aabb, color, worldTransform);

            //]std::cout << "- AABB min: " << aabb.GetBoundsMin();
            //]std::cout << "- AABB max: " << aabb.GetBoundsMax();
        }
       // std::cout << "\n";
    }

    void RenderMeshBvh(uint64_t bvhId, glm::vec4 color, glm::mat4 worldTransform) {
        if (!MeshBvhExists(bvhId)) return;

        MeshBvh* meshBvh = GetMeshBvhById(bvhId);

        for (BvhNode& node : meshBvh->m_nodes) {
            AABB aabb(node.boundsMin, node.boundsMax);
            Renderer::DrawAABB(aabb, color, worldTransform);
        }
    }
}