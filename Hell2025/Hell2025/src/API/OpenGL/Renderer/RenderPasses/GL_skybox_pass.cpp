#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Editor/Editor.h"
#include "Viewport/ViewportManager.h"

namespace OpenGLRenderer {

    void SkyBoxPass() {
        if (Editor::IsOpen()) return;

        OpenGLShader* shader = GetShader("Skybox");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLCubemapView* skyboxCubemapView = GetCubemapView("SkyboxNightSky");
        Mesh* mesh = AssetManager::GetCubeMesh();

        gBuffer->Bind();
        gBuffer->DrawBuffer("BaseColor");       
        shader->Bind();

        SetRasterizerState("SkyBox");
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapView->GetHandle());
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), 1, mesh->baseVertex, i);
            }
        }
    }
}