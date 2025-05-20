#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"`
#include "World/World.h"`

namespace OpenGLRenderer {

    void GlassPass() {
        SetRasterizerState("GlassPass");

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLShader* shader = GetShader("Glass");
        OpenGLShader* compositeShader = GetShader("GlassComposite");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        shader->Bind();

        gBuffer->Bind();
        gBuffer->DrawBuffer("Glass");

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        glBindTextureUnit(0, gBuffer->GetDepthAttachmentHandle());
        glBindTextureUnit(7, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());

        //OpenGLFrameBuffer* flashLightShadowMapFBO = GetFrameBuffer("FlashlightShadowMap");
        //glBindTextureUnit(8, flashLightShadowMapFBO->GetDepthAttachmentHandle());

        std::vector<Window>& windows = World::GetWindows();
        
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);
            shader->SetInt("u_viewportIndex", i);

            Player* player = Game::GetLocalPlayerByIndex(i);

            for (Window& window : windows) {           
                for (const RenderItem& renderItem : window.GetGlassRenderItems()) {                    
                    shader->SetMat4("u_modelMatrix", renderItem.modelMatrix);

                    Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                    if (!mesh) continue;

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());

                    glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
                }            
            }
        }
        
        // Composite that render back into the lighting texture
        gBuffer->SetViewport();
        compositeShader->Bind();
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glBindImageTexture(1, gBuffer->GetColorAttachmentHandleByName("Glass"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        glDispatchCompute(gBuffer->GetWidth() / 16, gBuffer->GetHeight() / 4, 1);

        glDepthMask(GL_TRUE);
    }
}