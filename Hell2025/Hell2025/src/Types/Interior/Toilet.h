#pragma once
#include "CreateInfo.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"

struct Toilet {
    Toilet() = default;
    Toilet(ToiletCreateInfo createInfo, SpawnOffset spawnOffset);
    void Update(float deltaTime);
    void CleanUp();

    //const glm::vec3& GetPosition() const { return m_position; }
    //const glm::mat4& GetModelMatrix() const { return m_modelMatrix; }
    const std::vector<RenderItem>& GetRenderItems() const { return m_meshNodes.GetRenderItems(); }

private:
    ToiletCreateInfo m_createInfo;
    Transform m_transform;
    MeshNodes m_meshNodes;

};