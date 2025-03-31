#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types/Renderer/Model.h"

struct Light {
    Light() = default;
    Light(LightCreateInfo createInfo);
    void Update(float deltaTime);

    LightCreateInfo GetCreateInfo();
    glm::vec3 GetPosition();
    glm::vec3 GetColor();
    float GetStrength();
    float GetRadius();
    void SetMousePickIndex(int index);
    void SetPosition(glm::vec3 position);

    const std::vector<RenderItem>& GetRenderItems() const   { return m_renderItems; }
    const uint64_t GetObjectId() const                      { return m_objectId; }

private:
    uint64_t m_objectId = 0;
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_color = glm::vec3(1, 0.7799999713897705, 0.5289999842643738);
    int m_mousePickIndex = 0;
    float m_radius = 6.0f;
    float m_strength = 1.0f;
    LightType m_type = LightType::LAMP_POST;
    Material* m_material = nullptr;
    Model* m_model0 = nullptr;
    Model* m_model1 = nullptr;
    Model* m_model2 = nullptr;
    Transform m_transform0;
    Transform m_transform1;
    Transform m_transform2;
    std::vector<RenderItem> m_renderItems;

    void UpdateRenderItems();
};