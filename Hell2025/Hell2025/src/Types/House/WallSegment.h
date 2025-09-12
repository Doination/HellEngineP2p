#pragma once
#include "Math/AABB.h"
#include "Modelling/Types/ClippingCube.h"
#include <vector>

struct WallSegment {
    void Init(glm::vec3 start, glm::vec3 end, float height, uint64_t parentObjectId, const SpawnOffset& spawnOffset);
    void SetMeshIndex(uint32_t index);
    void CleanUp();
    void CreateVertexData(std::vector<ClippingCube>& clippingCubes, float texOffsetX, float texOffsetY, float texScale);
    void CreatePhysicsObject();

    const glm::vec3& GetStart()                 const { return m_start; }
    const glm::vec3& GetEnd()                   const { return m_end; }
    const glm::vec3& GetNormal()                const { return m_normal; }
    const uint32_t GetMeshIndex()               const { return m_meshIndex; }
    const uint64_t GetObjectId()                const { return m_objectId; }
    const uint64_t GetParentObjectId()          const { return m_parentObjectId; }
    const float GetHeight()                     const { return m_height; }
    const AABB& GetAABB()                       const { return m_aabb; }
    const std::vector<glm::vec3>& GetCorners()  const { return m_corners; }
    const std::vector<Vertex>& GetVertices()    const { return m_vertices; }
    const std::vector<uint32_t>& GetIndices()   const { return m_indices; }
                                                               
private:
    glm::vec3 m_start;
    glm::vec3 m_end;
    glm::vec3 m_normal;
    float m_height = 2.4f;
    AABB m_aabb;
    uint64_t m_objectId = 0;
    uint64_t m_physicsId = 0;
    uint64_t m_parentObjectId = 0;
    std::vector<glm::vec3> m_corners;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    uint32_t m_meshIndex = 0;
    SpawnOffset m_spawnOffset;
};