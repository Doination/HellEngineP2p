#include "Tree.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Physics.h"
#include "UniqueID.h"
#include "Util.h"

Tree::Tree(TreeCreateInfo createInfo) {
    m_objectId = UniqueID::GetNext();

    m_transform.position = createInfo.position;
    m_transform.rotation = createInfo.rotation;
    m_transform.scale = createInfo.scale;// *2.0f;
    m_treeType = (TreeType)createInfo.type;

    float collisionCaspuleRadius = 0.0f;
    float collisionCaspuleHalfHeight = 0.0f;

    if (m_treeType == TreeType::TREE_LARGE_0) {
        m_model = AssetManager::GetModelByName("TreeLarge_0");
        m_meshNodes.InitFromModel(m_model);
        m_meshNodes.SetMaterialByMeshName("Tree", "TreeLarge_0");
        collisionCaspuleRadius = 0.4f;
        collisionCaspuleHalfHeight = 2.0f;

    }
    else if (m_treeType == TreeType::TREE_LARGE_1) {
        m_model = AssetManager::GetModelByName("TreeLarge_1");
        m_meshNodes.InitFromModel(m_model);
        m_meshNodes.SetMaterialByMeshName("Tree", "TreeLarge_1");
        collisionCaspuleRadius = 0.4f;
        collisionCaspuleHalfHeight = 2.0f;
    }
    else if (m_treeType == TreeType::TREE_LARGE_2) {
        m_model = AssetManager::GetModelByName("TreeLarge_2");
        m_meshNodes.InitFromModel(m_model);
        m_meshNodes.SetMaterialByMeshName("Tree", "TreeLarge_2");
        collisionCaspuleRadius = 0.4f;
        collisionCaspuleHalfHeight = 2.0f;
    }
    else if (m_treeType == TreeType::BLACK_BERRIES) {
        m_model = AssetManager::GetModelByName("BlackBerries");
        m_meshNodes.InitFromModel(m_model);
        m_meshNodes.SetMaterialByMeshName("Leaves", "Leaves_BlackBerry");
        m_meshNodes.SetBlendingModeByMeshName("Leaves", BlendingMode::ALPHA_DISCARDED);
        m_meshNodes.SetMaterialByMeshName("Trunk", "TreeLarge_0");
        m_meshNodes.SetMaterialByMeshName("Trunk2", "TreeLarge_0");
        collisionCaspuleRadius = 0.9f;
        collisionCaspuleHalfHeight = 0.4f;
    }
    m_meshNodes.SetObjectTypes(ObjectType::TREE);
    m_meshNodes.SetObjectIds(m_objectId);

    // Collision shape
    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_ENABLED;
    filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    filterData.collidesWith = (CollisionGroup)(CHARACTER_CONTROLLER | BULLET_CASING | ITEM_PICK_UP);
    
    // Create collision capsule
    Transform localOffset;
    localOffset.position.y = collisionCaspuleHalfHeight;
    localOffset.rotation.x = HELL_PI * 0.5f;
    localOffset.rotation.z = HELL_PI * 0.5f;
    m_rigidStaticId = Physics::CreateRigidStaticFromCapsule(m_transform, collisionCaspuleRadius, collisionCaspuleHalfHeight, filterData, localOffset);
}

TreeCreateInfo Tree::GetCreateInfo() {
    TreeCreateInfo createInfo;
    createInfo.position = m_transform.position;
    createInfo.rotation = m_transform.rotation;
    createInfo.scale = m_transform.scale;
    createInfo.type = m_treeType;
    return createInfo;
}

void Tree::BeginFrame() {
    m_isSelected = false;
}

void Tree::MarkAsSelected() {
    m_isSelected = true;
}

bool Tree::IsSelected() {
    return m_isSelected;
}

void Tree::Update(float deltaTime) {
    m_modelMatrix = m_transform.to_mat4();
    UpdateRenderItems();
}

void Tree::CleanUp() {
    Physics::MarkRigidStaticForRemoval(m_rigidStaticId);
}

void Tree::SetPosition(glm::vec3 position) {
    m_transform.position = position;
}

void Tree::UpdateRenderItems() {
    m_meshNodes.UpdateRenderItems(GetModelMatrix());
}
