#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Physics/Types/RigidDynamic.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"

struct GameObject {
    GameObject() = default;
    GameObject(GameObjectCreateInfo createInfo);
    GameObjectCreateInfo GetCreateInfo();

    std::string m_name;
    Transform m_transform;
    MeshNodes m_meshNodes;

    void CleanUp();
    void Update(float deltaTime);
    void SetName(const std::string& name);
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetRotationY(float rotation);
    void SetScale(glm::vec3 scale);
    void SetModel(const std::string& name);
    void SetMeshMaterial(const std::string& meshName, const std::string& materialName);
    void SetMeshBlendingMode(const std::string& meshName, BlendingMode blendingMode);
    void PrintMeshNames();
    void UpdateRenderItems();
    void SetConvexHullsFromModel(const std::string modelName);
    
    void BeginFrame();
    void MarkAsSelected();
    bool IsSelected();

    glm::vec3 GetPosition() const;
    glm::vec3 GetEulerRotation() const;
    glm::vec3 GetScale() const;
    const glm::mat4 GetModelMatrix();

    const GameObjectCreateInfo& GetCreateInfo() const                       { return m_createInfo; }
    const std::vector<RenderItem>& GetRenderItems() const                   { return m_meshNodes.GetRenderItems(); }
    const std::vector<RenderItem>& GetRenderItemsBlended()const             { return m_meshNodes.GetRenderItemsBlended(); }
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() const     { return m_meshNodes.GetRenderItemsAlphaDiscarded(); }
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer() const       { return m_meshNodes.GetRenderItemsHairTopLayer(); }
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer() const    { return m_meshNodes.GetRenderItemsHairBottomLayer(); }
    const uint64_t GetObjectId() const                                      { return m_objectId; }

private:
    GameObjectCreateInfo m_createInfo;
    uint64_t m_physicsId = 0;
    uint64_t m_objectId = 0;
    bool m_selected = false;
    bool m_hasPhysics = false;   
};