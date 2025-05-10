#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "API/OpenGL/GL_backend.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"

namespace OpenGLRenderer {

    void UpdateHairDebugInput() {
        RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
        int peelCount = renderSettings.depthPeelCount;
        if (Input::KeyPressed(HELL_KEY_8) && peelCount < 7) {
            renderSettings.depthPeelCount++;
            std::cout << "Depth peel layer count: " << renderSettings.depthPeelCount << "\n";
        }
        if (Input::KeyPressed(HELL_KEY_9) && peelCount > 0) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            renderSettings.depthPeelCount--;
            std::cout << "Depth peel layer count: " << renderSettings.depthPeelCount << "\n";
        }
    }

    void HairPass() {
        UpdateHairDebugInput();

        OpenGLShader* shader = GetShader("HairFinalComposite");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* hairFrameBuffer = GetFrameBuffer("Hair");

        if (!shader) return;
        if (!gBuffer) return;
        if (!hairFrameBuffer) return;

        // Setup state
        hairFrameBuffer->Bind();
        hairFrameBuffer->ClearAttachment("Composite", 0, 0, 0, 0);
        hairFrameBuffer->SetViewport();

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();
        RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();

        // Render all top then all Bottom layers
        RenderHairLayer(&drawInfoSet.hairTopLayer, renderSettings.depthPeelCount);
        RenderHairLayer(&drawInfoSet.hairBottomLayer, renderSettings.depthPeelCount);

        shader->Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hairFrameBuffer->GetColorAttachmentHandleByName("Composite"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer->GetColorAttachmentHandleByName("FinalLighting"));





        OpenGLFrameBuffer* waterFrameBuffer = GetFrameBuffer("Water");
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, waterFrameBuffer->GetColorAttachmentHandleByName("Color"));





        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
    }

    void RenderHairLayer(const std::vector<DrawIndexedIndirectCommand>(*drawCommands)[4], int peelCount) {
        const Resolutions& resolutions = Config::GetResolutions();
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* hairFrameBuffer = GetFrameBuffer("Hair");
        OpenGLShader* depthPeelShader = GetShader("HairDepthPeel");
        OpenGLShader* hairLightingShader = GetShader("HairLighting");
        OpenGLShader* hairLayerCompositeShader = GetShader("HairLayerComposite");

        if (!gBuffer) return;
        if (!hairFrameBuffer) return;
        if (!depthPeelShader) return;
        if (!hairLightingShader) return;
        if (!hairLayerCompositeShader) return;

        hairFrameBuffer->Bind();
        hairFrameBuffer->ClearAttachment("ViewspaceDepthPrevious", 1.0f);

        static bool test = false;
        test = (Input::KeyDown(HELL_KEY_T));

        for (int j = 0; j < peelCount; j++) {

            // Viewspace depth pass
            depthPeelShader->Bind();
            glBindTextureUnit(3, hairFrameBuffer->GetColorAttachmentHandleByName("ViewspaceDepthPrevious"));
            SetRasterizerState("HairViewspaceDepth");
            OpenGLRenderer::BlitFrameBufferDepth(gBuffer, hairFrameBuffer);

            for (int i = 0; i < 4; i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (viewport->IsVisible()) {
                    OpenGLRenderer::SetViewport(hairFrameBuffer, viewport);
                    hairFrameBuffer->DrawBuffer("ViewspaceDepth");

                    if (BackEnd::RenderDocFound()) {
                        SplitMultiDrawIndirect(depthPeelShader, (*drawCommands)[i]);
                    }
                    else {
                        MultiDrawIndirect((*drawCommands)[i]);
                    }
                }
            }
            // Color pass
            hairFrameBuffer->ClearAttachment("Lighting", 0.0f, 0.0f, 0.0f, 0.0f);
            hairFrameBuffer->DrawBuffers({ "Lighting", "ViewspaceDepthPrevious" });

            hairLightingShader->Bind();
            glBindTextureUnit(3, hairFrameBuffer->GetColorAttachmentHandleByName("ViewspaceDepth"));
            glBindTextureUnit(4, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
            SetRasterizerState("HairLighting");

            for (int i = 0; i < 4; i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (viewport->IsVisible()) {
                    OpenGLRenderer::SetViewport(hairFrameBuffer, viewport);

                    if (BackEnd::RenderDocFound()) {
                        SplitMultiDrawIndirect(hairLightingShader, (*drawCommands)[i]);
                    }
                    else {
                        MultiDrawIndirect((*drawCommands)[i]);
                    }
                }
            }
            // Composite
            hairLayerCompositeShader->Bind();
            glBindImageTexture(0, hairFrameBuffer->GetColorAttachmentHandleByName("Lighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            glBindImageTexture(1, hairFrameBuffer->GetColorAttachmentHandleByName("Composite"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            int workGroupsX = (hairFrameBuffer->GetWidth() + 7) / 8;
            int workGroupsY = (hairFrameBuffer->GetHeight() + 7) / 8;
            glDispatchCompute(workGroupsX, workGroupsY, 1);
        }
    }
}