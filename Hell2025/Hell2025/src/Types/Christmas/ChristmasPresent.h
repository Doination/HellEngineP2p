#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types\Renderer\Model.h"

struct ChristmasPresent {
    ChristmasPresent() = default;
    ChristmasPresent(const ChristmasPresentCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    void Update(float deltaTime);
    void CleanUp();
    void CreateRenderItems();

    const glm::vec3& GetPosition() const                    { return m_position; }
    const glm::mat4& GetModelMatrix() const                 { return m_modelMatrix; }
    const std::vector<RenderItem>& GetRenderItems() const   { return m_renderItems; }

private:
    void CreatePhysicsObject();

    uint64_t m_rigidDynamicId = 0;
    uint64_t m_objectId = 0;
    ChristmasPresentCreateInfo m_createInfo;
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    Model* m_model = nullptr;
    Material* m_material = nullptr;
    std::vector<RenderItem> m_renderItems;

    
};