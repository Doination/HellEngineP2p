#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types/Renderer/Model.h"

struct Door {
    void Init(DoorCreateInfo createInfo);
    void SetPosition(glm::vec3 position);
    void Update(float deltaTime);
    void CleanUp();
    void UpdateRenderItems(); 
    void SubmitRenderItems();
    void Interact();

    const bool MovedThisFrame() const                       { return m_movedThisFrame; }
    const uint64_t GetObjectId() const                      { return m_objectId; }
    const uint64_t GetFrameObjectId() const                 { return m_frameObjectId; }
    const glm::vec3& GetPosition() const                    { return m_position; }
    const glm::vec3& GetRotation() const                    { return m_rotation; }
    const glm::vec3& GetInteractPosition() const            { return m_interactPosition; }
    const glm::mat4& GetDoorModelMatrix () const            { return m_doorModelMatrix; }
    const glm::mat4& GetDoorFrameModelMatrix () const       { return m_frameModelMatrix; }
    const Model* GetDoorModel() const                       { return m_doorModel; }
    const Model* GetDoorFrameModel() const                  { return m_frameModel; }
    const OpeningState& GetOpeningState() const             { return m_openingState; }
    const DoorCreateInfo& GetCreateInfo() const             { return m_createInfo; }
    const std::vector<RenderItem>& GetRenderItems() const   { return m_renderItems; }

private:
    DoorCreateInfo m_createInfo;
    bool m_movedThisFrame = true;
    uint64_t m_lifeTime = 0;
    uint64_t m_objectId = 0;
    uint64_t m_frameObjectId = 0;
    uint64_t m_physicsId = 0;
    Model* m_doorModel = nullptr;
    Model * m_frameModel = nullptr;
    Material* m_material = nullptr;
    OpeningState m_openingState = OpeningState::CLOSED;
    std::vector<RenderItem> m_renderItems;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_interactPosition;
    float m_currentOpenRotation = 0;
    float m_maxOpenRotation = 1.8f;
    glm::mat4 m_doorModelMatrix = glm::mat4(1.0f);
    glm::mat4 m_frameModelMatrix = glm::mat4(1.0f);
};