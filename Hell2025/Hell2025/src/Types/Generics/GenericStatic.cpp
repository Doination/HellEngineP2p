#include "GenericStatic.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Util/Util.h"

GenericStatic::GenericStatic(const GenericStaticCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_position = createInfo.position + spawnOffset.translation;
    m_rotation = createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);
    m_createInfo = createInfo;

    Transform transform;
    transform.position = m_position;
    transform.rotation = m_rotation;
    m_modelMatrix = transform.to_mat4();

    CreateRenderItems();
}

void GenericStatic::CreateRenderItems() {
    // Build render items
    m_renderItems.clear();

    if (m_createInfo.type == GenericStaticType::COUCH) {
        m_model = AssetManager::GetModelByName("Couch");
        m_material = AssetManager::GetMaterialByName("Couch");
    }

    if (!m_model) {
        std::cout << "Could not get GenericStaticObject model\n";
        return;
    }

    if (!m_material) {
        std::cout << "Could not get GenericStaticObject mateial\n";
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

void GenericStatic::CleanUp() {
    // Nothing as of yet
}
