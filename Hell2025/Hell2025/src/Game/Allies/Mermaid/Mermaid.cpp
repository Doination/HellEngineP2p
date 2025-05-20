#include "Mermaid.h"

void Mermaid::Init(MermaidCreateInfo createInfo, SpawnOffset spawnOffset) {
    m_createInfo = createInfo;
    m_spawnOffset = spawnOffset;

    m_transform.position = m_createInfo.position + m_spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, m_spawnOffset.yRotation, 0.0f);

    m_meshNodes.InitFromModel("Mermaid");
    m_meshNodes.SetMaterialByMeshName("Arms", "MermaidArms");
    m_meshNodes.SetMaterialByMeshName("Body", "MermaidBody");
    m_meshNodes.SetMaterialByMeshName("BoobTube", "BoobTube");
    m_meshNodes.SetMaterialByMeshName("EyelashLower", "MermaidLashes");
    m_meshNodes.SetMaterialByMeshName("EyelashUpper", "MermaidLashes");
    m_meshNodes.SetMaterialByMeshName("EyeLeft", "MermaidEye");
    m_meshNodes.SetMaterialByMeshName("EyeRight", "MermaidEye");
    m_meshNodes.SetMaterialByMeshName("Face", "MermaidFace");
    m_meshNodes.SetMaterialByMeshName("HairInner", "MermaidHair");
    m_meshNodes.SetMaterialByMeshName("HairOutta", "MermaidHair");
    m_meshNodes.SetMaterialByMeshName("HairScalp", "MermaidScalp");
    m_meshNodes.SetMaterialByMeshName("Nails", "Nails");
    m_meshNodes.SetMaterialByMeshName("Rock", "Rock");
    m_meshNodes.SetMaterialByMeshName("Tail", "MermaidTail");
    m_meshNodes.SetMaterialByMeshName("TailFin", "MermaidTail");

    m_meshNodes.SetBlendingModeByMeshName("EyelashLower", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("EyelashUpper", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("HairScalp", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("HairOutta", BlendingMode::HAIR_TOP_LAYER);
    m_meshNodes.SetBlendingModeByMeshName("HairInner", BlendingMode::HAIR_UNDER_LAYER);
}

void Mermaid::Update(float deltaTime) {
    UpdateRenderItems();
}

void Mermaid::UpdateRenderItems() {
    m_meshNodes.UpdateHierachy();
    m_meshNodes.UpdateRenderItems(m_transform.to_mat4());
}

void Mermaid::CleanUp() {
    // Nothing as of yet
}