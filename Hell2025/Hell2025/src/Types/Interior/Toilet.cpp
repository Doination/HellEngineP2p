#include "Toilet.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/Renderer.h"

Toilet::Toilet(ToiletCreateInfo createInfo, SpawnOffset spawnOffset) {
    m_createInfo = createInfo;
    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);

    static Model* model = AssetManager::GetModelByName("Toilet");
    if (!model) {
        std::cout << "Toilet() constructor failed because your hack to load the model into a static Model pointer failed the first time around\n";
    }

    m_meshNodes.InitFromModel(model);
}

void Toilet::Update(float deltaTime) {
    m_meshNodes.UpdateRenderItems(m_transform.to_mat4());
}

void Toilet::CleanUp() {

}