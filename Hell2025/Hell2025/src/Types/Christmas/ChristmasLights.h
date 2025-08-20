#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types/Renderer/MeshBuffer.h"
//#include "API/OpenGL/Types/GL_detachedMesh.hpp"

struct ChristmasLights {
    ChristmasLights() = default;
    ChristmasLights(const ChristmasLightsCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    void Update(float deltaTime);
    void CleanUp();

    glm::vec3 m_start;
    glm::vec3 m_end;
    float m_sag = 1.0f;
    bool m_spiral = false;
    glm::vec3 sprialTopCenter;
    float spiralRadius;
    float spiralHeight;
    float m_time = 0;

    MeshBuffer m_meshBuffer;
    //OpenGLDetachedMesh g_wireMesh;
    std::vector<glm::vec3> m_wireSegmentPoints;
    std::vector<glm::vec3> m_lightSpawnPoints;

    const std::vector<RenderItem>& GetRenderItems() const { return m_renderItems; }

private:
    std::vector<RenderItem> m_renderItems;
};