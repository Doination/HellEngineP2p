#include "GenericBouncable.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Util/Util.h"

GenericBouncable::GenericBouncable(const GenericBouncableCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);
    m_createInfo = createInfo;

    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;
    m_modelMatrix = transform.to_mat4();

    CreateRenderItems();
}

void GenericBouncable::CreateRenderItems() {
    // Build render items
    m_renderItems.clear();

    if (m_createInfo.type == GenericBouncableType::COUCH_CUSHION_0) {
        m_model = AssetManager::GetModelByName("CouchCushion0");
        m_material = AssetManager::GetMaterialByName("Couch");
    }
    if (m_createInfo.type == GenericBouncableType::COUCH_CUSHION_1) {
        m_model = AssetManager::GetModelByName("CouchCushion1");
        m_material = AssetManager::GetMaterialByName("Couch");
    }
    if (m_createInfo.type == GenericBouncableType::COUCH_CUSHION_2) {
        m_model = AssetManager::GetModelByName("CouchCushion2");
        m_material = AssetManager::GetMaterialByName("Couch");
    }
    if (m_createInfo.type == GenericBouncableType::COUCH_CUSHION_3) {
        m_model = AssetManager::GetModelByName("CouchCushion3");
        m_material = AssetManager::GetMaterialByName("Couch");
    }
    if (m_createInfo.type == GenericBouncableType::COUCH_CUSHION_4) {
        m_model = AssetManager::GetModelByName("CouchCushion4");
        m_material = AssetManager::GetMaterialByName("Couch");
    }

    if (!m_model) {
        std::cout << "Could not get GenericStaticObject model\n";
        return;
    }

    if (!m_material) {
        std::cout << "Could not get GenericStaticObject material\n";
        return;
    }

    for (uint32_t meshIndex : m_model->GetMeshIndices()) {
        RenderItem& renderItem = m_renderItems.emplace_back();
        renderItem.objectType = (int)ObjectType::GENERIC_STATIC;
        renderItem.modelMatrix = m_modelMatrix;
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        renderItem.castShadows = false;
        if (m_material) {
            renderItem.baseColorTextureIndex = m_material->m_basecolor;
            renderItem.normalMapTextureIndex = m_material->m_normal;
            renderItem.rmaTextureIndex = m_material->m_rma;
        }
        Util::UpdateRenderItemAABB(renderItem);
    }
}

void GenericBouncable::CleanUp() {
    // Nothing as of yet
}
