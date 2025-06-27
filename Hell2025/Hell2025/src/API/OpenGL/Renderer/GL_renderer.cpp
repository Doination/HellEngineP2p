﻿#include "GL_renderer.h"
#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/GL_Util.h"
#include "API/OpenGL/Types/GL_indirectBuffer.hpp"
#include "API/OpenGL/Types/GL_pbo.hpp"
#include "API/OpenGL/Types/GL_shader.h"
#include "API/OpenGL/Types/GL_ssbo.hpp"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/BackEnd.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Config/Config.h"
#include "Input/Input.h"
#include "Ocean/Ocean.h"
#include "Player/Player.h"
#include "Renderer/RenderDataManager.h"
#include "Util/Util.h"
#include "UI/UIBackEnd.h"
#include "UI/TextBlitter.h"
#include "Types/Game/GameObject.h"
#include "../Timer.hpp"
#include <glm/gtx/matrix_decompose.hpp>

#include "../Editor/Editor.h"
#include "../Editor/Gizmo.h"
#include "../Renderer/RenderDataManager.h"
#include "../Viewport/ViewportManager.h"

#include "Input/Input.h"
#include "API/OpenGL/Types/GL_texture_readback.h"
#include "Tools/ImageTools.h"

namespace OpenGLRenderer {
    std::unordered_map<std::string, OpenGLShader> g_shaders;
    std::unordered_map<std::string, OpenGLFrameBuffer> g_frameBuffers;
    std::unordered_map<std::string, OpenGLShadowMap> g_shadowMaps;
    std::unordered_map<std::string, OpenGLCubemapView> g_cubemapViews;
    std::unordered_map<std::string, OpenGLSSBO> g_ssbos;
    std::unordered_map<std::string, OpenGLRasterizerState> g_rasterizerStates;
    std::unordered_map<std::string, OpenGLShadowCubeMapArray> g_shadowCubeMapArrays;
    std::unordered_map<std::string, OpenGLShadowMapArray> g_shadowMapArrays;
    std::unordered_map<std::string, OpenGLTextureArray> g_textureArrays;

    OpenGLMeshPatch g_tesselationPatch;

    OpenGLFrameBuffer g_blurBuffers[4][4] = {};

    //std::vector<float> g_shadowCascadeLevels{ FAR_PLANE / 50.0f, FAR_PLANE / 25.0f, FAR_PLANE / 10.0f, FAR_PLANE / 2.0f };
    std::vector<float> g_shadowCascadeLevels{ 5.0f, 10.0f, 20.0f, 40.0f };
    const glm::vec3 g_lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
    unsigned int g_lightFBO;
    unsigned int g_lightDepthMaps;
    constexpr unsigned int g_depthMapResolution = 4096;

    int g_fftDisplayMode = 0;
    int g_fftEditBand = 0;
    
    void LoadShaders();

    IndirectBuffer g_indirectBuffer;

    struct Cubemaps {
        OpenGLCubemapView g_skyboxView;
    } g_cubemaps;

    void ClearRenderTargets();

    int GetFftDisplayMode() {
        return g_fftDisplayMode;
    }

    void Init() {
        const Resolutions& resolutions = Config::GetResolutions();

        Ocean::Init();

        g_textureArrays["WoundMasks"] = OpenGLTextureArray();
        g_textureArrays["WoundMasks"].AllocateMemory(WOUND_MASK_TEXTURE_SIZE, WOUND_MASK_TEXTURE_SIZE, GL_RGBA8, 1, 6);

        g_frameBuffers["DecalPainting"] = OpenGLFrameBuffer("DecalPainting", 512, 512);
        g_frameBuffers["DecalPainting"].CreateAttachment("UVMap", GL_RGBA8, GL_LINEAR, GL_LINEAR);
        g_frameBuffers["DecalPainting"].CreateDepthAttachment(GL_DEPTH_COMPONENT24);

        g_frameBuffers["DecalMasks"] = OpenGLFrameBuffer("DecalMasks", 256, 256);
        g_frameBuffers["DecalMasks"].CreateAttachment("DecalMask0", GL_RGBA8, GL_LINEAR, GL_LINEAR);

        //g_frameBuffers["KanagarooDecalMap"].CreateAttachment("DecalMask", GL_RGBA8, GL_LINEAR, GL_LINEAR);

        g_frameBuffers["GBuffer"] = OpenGLFrameBuffer("GBuffer", resolutions.gBuffer);
        g_frameBuffers["GBuffer"].CreateAttachment("BaseColor", GL_RGBA8);
        g_frameBuffers["GBuffer"].CreateAttachment("Normal", GL_RGBA16F);
        g_frameBuffers["GBuffer"].CreateAttachment("RMA", GL_RGBA8);        // In alpha is screenspace blood decal mask
        g_frameBuffers["GBuffer"].CreateAttachment("FinalLighting", GL_RGBA16F);
        g_frameBuffers["GBuffer"].CreateAttachment("WorldPosition", GL_RGBA32F);
        g_frameBuffers["GBuffer"].CreateAttachment("Emissive", GL_RGBA8);
        g_frameBuffers["GBuffer"].CreateAttachment("Glass", GL_RGBA8);
        g_frameBuffers["GBuffer"].CreateDepthAttachment(GL_DEPTH_COMPONENT32F);

        g_frameBuffers["QuarterSize"].Create("QuarterSize", resolutions.gBuffer.x / 4, resolutions.gBuffer.y / 4);
        g_frameBuffers["QuarterSize"].CreateAttachment("DownsampledFinalLighting", GL_RGBA16F);

        g_frameBuffers["MiscFullSize"].Create("FullSize", resolutions.gBuffer);
        g_frameBuffers["MiscFullSize"].CreateAttachment("GaussianFinalLightingIntermediate", GL_RGBA16F);
        g_frameBuffers["MiscFullSize"].CreateAttachment("GaussianFinalLighting", GL_RGBA16F);
        g_frameBuffers["MiscFullSize"].CreateAttachment("ScreenSpaceBloodDecalMask", GL_R8);
        g_frameBuffers["MiscFullSize"].CreateAttachment("RaytracedScene", GL_RGBA8);
              
        g_frameBuffers["Water"] = OpenGLFrameBuffer("Water", resolutions.gBuffer);
        g_frameBuffers["Water"].CreateAttachment("Color", GL_RGBA16F);
        g_frameBuffers["Water"].CreateAttachment("UnderwaterMask", GL_R8);
        g_frameBuffers["Water"].CreateAttachment("WorldPosition", GL_RGBA32F);
        g_frameBuffers["Water"].CreateDepthAttachment(GL_DEPTH_COMPONENT32F);

        g_frameBuffers["WIP"] = OpenGLFrameBuffer("WIP", resolutions.gBuffer);
        g_frameBuffers["WIP"].CreateAttachment("WorldPosition", GL_RGBA32F);

        g_frameBuffers["Outline"] = OpenGLFrameBuffer("Outline", resolutions.gBuffer);
        g_frameBuffers["Outline"].CreateAttachment("Mask", GL_R8);
        g_frameBuffers["Outline"].CreateAttachment("Result", GL_R8);

        g_frameBuffers["Hair"] = OpenGLFrameBuffer("Hair", resolutions.hair);
        g_frameBuffers["Hair"].CreateDepthAttachment(GL_DEPTH_COMPONENT32F);
        g_frameBuffers["Hair"].CreateAttachment("Lighting", GL_RGBA16F);
        g_frameBuffers["Hair"].CreateAttachment("ViewspaceDepth", GL_R32F);
        g_frameBuffers["Hair"].CreateAttachment("ViewspaceDepthPrevious", GL_R32F);
        g_frameBuffers["Hair"].CreateAttachment("Composite", GL_RGBA16F);

        g_frameBuffers["FinalImage"] = OpenGLFrameBuffer("FinalImage", resolutions.finalImage);
        g_frameBuffers["FinalImage"].CreateAttachment("Color", GL_RGBA16F);
        g_frameBuffers["FinalImage"].CreateAttachment("ViewportIndex", GL_R8UI);

        g_frameBuffers["UI"] = OpenGLFrameBuffer("UI", resolutions.ui);
        g_frameBuffers["UI"].CreateAttachment("Color", GL_RGBA8, GL_NEAREST, GL_NEAREST);

        g_frameBuffers["World"] = OpenGLFrameBuffer("World", 1, 1);
        g_frameBuffers["World"].CreateAttachment("HeightMap", GL_R16F);

        g_frameBuffers["HeightMapBlitBuffer"] = OpenGLFrameBuffer("HeightMapBlitBuffer", HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE);

        g_frameBuffers["HeightMap"] = OpenGLFrameBuffer("HeightMap", HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE);
        g_frameBuffers["HeightMap"].CreateAttachment("Color", GL_R16F);

        //g_frameBuffers["FlashlightShadowMap"] = OpenGLFrameBuffer("Flashlight", FLASHLIGHT_SHADOWMAP_SIZE, FLASHLIGHT_SHADOWMAP_SIZE);
        //g_frameBuffers["FlashlightShadowMap"].CreateDepthAttachment(GL_DEPTH32F_STENCIL8, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, glm::vec4(1.0f));

        g_frameBuffers["FFT_band0"].Create("FFT_band0", Ocean::GetFFTResolution(0).x, Ocean::GetFFTResolution(0).y);
        g_frameBuffers["FFT_band0"].CreateAttachment("Displacement", GL_RGBA32F, GL_LINEAR, GL_LINEAR, GL_REPEAT);
        g_frameBuffers["FFT_band0"].CreateAttachment("Normals", GL_RGBA32F, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, true);

        g_frameBuffers["FFT_band1"].Create("FFT_band1", Ocean::GetFFTResolution(1).x, Ocean::GetFFTResolution(1).y);
        g_frameBuffers["FFT_band1"].CreateAttachment("Displacement", GL_RGBA32F, GL_LINEAR, GL_LINEAR, GL_REPEAT, true);
        g_frameBuffers["FFT_band1"].CreateAttachment("Normals", GL_RGBA32F, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, true);

        g_shadowMaps["FlashlightShadowMaps"] = OpenGLShadowMap("FlashlightShadowMaps", FLASHLIGHT_SHADOWMAP_SIZE, FLASHLIGHT_SHADOWMAP_SIZE, 4);

        GLbitfield staticFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
        GLbitfield dynamicFlags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

        // Create ssbos
        g_ssbos["Samplers"] = OpenGLSSBO(sizeof(glm::uvec2), GL_DYNAMIC_STORAGE_BIT);
        g_ssbos["ViewportData"] = OpenGLSSBO(sizeof(ViewportData) * 4, GL_DYNAMIC_STORAGE_BIT);
        g_ssbos["RendererData"] = OpenGLSSBO(sizeof(RendererData), GL_DYNAMIC_STORAGE_BIT);
        g_ssbos["InstanceData"] = OpenGLSSBO(sizeof(RenderItem) * MAX_INSTANCE_DATA_COUNT, GL_DYNAMIC_STORAGE_BIT);
        g_ssbos["SkinningTransforms"] = OpenGLSSBO(sizeof(glm::mat4) * MAX_ANIMATED_TRANSFORMS, GL_DYNAMIC_STORAGE_BIT);
        g_ssbos["CSMLightProjViewMatrices"] = OpenGLSSBO(sizeof(glm::mat4) * MAX_VIEWPORT_COUNT * SHADOW_CASCADE_COUNT, GL_DYNAMIC_STORAGE_BIT);
        g_ssbos["Lights"] = OpenGLSSBO(sizeof(GPULight) * MAX_GPU_LIGHTS, GL_DYNAMIC_STORAGE_BIT);
        g_ssbos["ScreenSpaceBloodDecals"] = OpenGLSSBO(sizeof(ScreenSpaceBloodDecalInstanceData) * MAX_SCREEN_SPACE_BLOOD_DECAL_COUNT, GL_DYNAMIC_STORAGE_BIT);


        //g_ssbos["ffth0"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), staticFlags);

        g_ssbos["ffth0Band0"] = OpenGLSSBO(Ocean::GetFFTResolution(0).x * Ocean::GetFFTResolution(0).y * sizeof(std::complex<float>), staticFlags);
        g_ssbos["ffth0Band1"] = OpenGLSSBO(Ocean::GetFFTResolution(1).x * Ocean::GetFFTResolution(1).y * sizeof(std::complex<float>), staticFlags);

        const glm::uvec2 oceanSize = Ocean::GetBaseFFTResolution(); // WARNING!!! This size must bit your largest FFT dimensions
        g_ssbos["fftSpectrumInSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftSpectrumOutSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftDispInXSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftDispZInSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftGradXInSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftGradZInSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftDispXOutSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftDispZOutSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftGradXOutSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["fftGradZOutSSBO"] = OpenGLSSBO(oceanSize.x * oceanSize.y * sizeof(std::complex<float>), dynamicFlags);
        g_ssbos["OceanPatchTransforms"] = OpenGLSSBO(sizeof(glm::mat4(1.0f)), GL_DYNAMIC_STORAGE_BIT);

        int dummySize = 64;
        CreateSSBO("TriangleData", dummySize, GL_DYNAMIC_STORAGE_BIT);
        CreateSSBO("SceneBvh", dummySize, GL_DYNAMIC_STORAGE_BIT);
        CreateSSBO("MeshesBvh", dummySize, GL_DYNAMIC_STORAGE_BIT);
        CreateSSBO("EntityInstances", dummySize, GL_DYNAMIC_STORAGE_BIT);

        g_tesselationPatch.Resize2(Ocean::GetTesslationMeshSize().x, Ocean::GetTesslationMeshSize().y);

        // Upload HO
        const std::vector<std::complex<float>>& h0Band0 = Ocean::GetH0(0);
        const std::vector<std::complex<float>>& h0Band1 = Ocean::GetH0(1);
        g_ssbos["ffth0Band0"].CopyFrom(h0Band0.data(), sizeof(std::complex<float>) * h0Band0.size());
        g_ssbos["ffth0Band1"].CopyFrom(h0Band1.data(), sizeof(std::complex<float>) * h0Band1.size());


        int tileXCount = g_frameBuffers["GBuffer"].GetWidth() / TILE_SIZE;
        int tileYCount = g_frameBuffers["GBuffer"].GetHeight() / TILE_SIZE;
        int tileCount = tileXCount * tileYCount;
        g_ssbos["TileLightData"] = OpenGLSSBO(tileCount * sizeof(TileLightData), GL_DYNAMIC_STORAGE_BIT);

        // Preallocate the indirect command buffer
        g_indirectBuffer.PreAllocate(sizeof(DrawIndexedIndirectCommand) * MAX_INDIRECT_DRAW_COMMAND_COUNT);

        LoadShaders();

        // Allocate shadow map array memory
        g_shadowCubeMapArrays["HiRes"] = OpenGLShadowCubeMapArray();
        g_shadowCubeMapArrays["HiRes"].Init(SHADOWMAP_HI_RES_COUNT, SHADOW_MAP_HI_RES_SIZE);

        // Moon light shadow maps
        float depthMapResolution = SHADOW_MAP_CSM_SIZE; 
        int layerCount = g_shadowCascadeLevels.size() + 1;
        g_shadowMapArrays["MoonlightPlayer1"] = OpenGLShadowMapArray();
        g_shadowMapArrays["MoonlightPlayer1"].Init(layerCount, depthMapResolution, GL_DEPTH_COMPONENT32F);

        InitGrass();
        InitOceanHeightReadback();
    }

    void InitMain() {
        InitRasterizerStates();

        // Attempt to load skybox
        std::vector<Texture*> textures = {
            AssetManager::GetTextureByName("px"),
            AssetManager::GetTextureByName("nx"),
            AssetManager::GetTextureByName("py"),
            AssetManager::GetTextureByName("ny"),
            AssetManager::GetTextureByName("pz"),
            AssetManager::GetTextureByName("nz"),
            //AssetManager::GetTextureByName("NightSky_Right"),
            //AssetManager::GetTextureByName("NightSky_Left"),
            //AssetManager::GetTextureByName("NightSky_Top"),
            //AssetManager::GetTextureByName("NightSky_Bottom"),
            //AssetManager::GetTextureByName("NightSky_Front"),
            //AssetManager::GetTextureByName("NightSky_Back"),
        };
        std::vector<GLuint> texturesHandles;
        for (Texture* texture : textures) {
            if (!texture) continue;
            texturesHandles.push_back(texture->GetGLTexture().GetHandle());
        }
        if (texturesHandles.size() == 6) {
            g_cubemapViews["SkyboxNightSky"] = OpenGLCubemapView(texturesHandles);
        }

        CreateBlurBuffers();
    }

    void LoadShaders() {

        g_shaders["RaytraceScene"] = OpenGLShader({ "GL_raytrace_scene.comp" });

        g_shaders["BlurHorizontal"] = OpenGLShader({ "GL_blur_horizontal.vert", "GL_blur.frag" });
        g_shaders["BlurVertical"] = OpenGLShader({ "GL_blur_vertical.vert", "GL_blur.frag" });
        g_shaders["BloodScreenSpaceDecalsComposite"] = OpenGLShader({ "GL_blood_screenspace_composite.comp" });
        g_shaders["BloodScreenSpaceDecalsMask"] = OpenGLShader({ "GL_blood_screenspace_decals_mask.vert", "GL_blood_screenspace_decals_mask.frag" });
        g_shaders["ComputeSkinning"] = OpenGLShader({ "GL_compute_skinning.comp" });
        g_shaders["DebugPointCloud"] = OpenGLShader({ "GL_debug_point_cloud.vert", "GL_debug_point_cloud.frag" });
        g_shaders["DebugSolidColor"] = OpenGLShader({ "GL_debug_solid_color.vert", "GL_debug_solid_color.frag" });
        g_shaders["DebugTextured"] = OpenGLShader({ "GL_debug_textured.vert", "GL_debug_textured.frag" });
        g_shaders["DebugView"] = OpenGLShader({ "GL_debug_view.comp" });
        g_shaders["DebugTileView"] = OpenGLShader({ "GL_debug_tile_view.comp" });
        g_shaders["DebugVertex"] = OpenGLShader({ "gl_debug_vertex.vert", "gl_debug_vertex.frag" });
        g_shaders["DecalPaintUVs"] = OpenGLShader({ "gl_decal_paint_uvs.vert", "gl_decal_paint_uvs.frag" });
        g_shaders["DecalPaintMask"] = OpenGLShader({ "gl_decal_paint_mask.comp" });
        g_shaders["Decals"] = OpenGLShader({ "GL_decals.vert", "GL_decals.frag" });
        g_shaders["EditorMesh"] = OpenGLShader({ "GL_editor_mesh.vert", "GL_editor_mesh.frag" });
        g_shaders["FttRadix64Vertical"] = OpenGLShader({ "GL_ftt_radix_64_vertical.comp" });
        g_shaders["FttRadix8Vertical"] = OpenGLShader({ "GL_ftt_radix_8_vertical.comp" });
        g_shaders["FttRadix64Horizontal"] = OpenGLShader({ "GL_ftt_radix_64_horizontal.comp" });
        g_shaders["FttRadix8Horizontal"] = OpenGLShader({ "GL_ftt_radix_8_horizontal.comp" });
        g_shaders["Fur"] = OpenGLShader({ "GL_fur.vert", "GL_fur.frag" });
        g_shaders["FurComposite"] = OpenGLShader({ "GL_fur_composite.comp" });
        g_shaders["EmissiveComposite"] = OpenGLShader({ "GL_emissive_composite.comp" });
        g_shaders["GBuffer"] = OpenGLShader({ "GL_GBuffer.vert", "GL_gBuffer.frag" });
        g_shaders["Gizmo"] = OpenGLShader({ "GL_gizmo.vert", "GL_gizmo.frag" });
        g_shaders["Glass"] = OpenGLShader({ "GL_glass.vert", "GL_glass.frag" });
        g_shaders["GlassComposite"] = OpenGLShader({ "GL_glass_composite.comp" });
        g_shaders["Grass"] = OpenGLShader({ "GL_grass.vert", "GL_grass.frag" });
        g_shaders["GrassGeometryGeneration"] = OpenGLShader({ "GL_grass_geometry_generation.comp" });
        g_shaders["GrassPositionGeneration"] = OpenGLShader({ "GL_grass_position_generation.comp" });
        g_shaders["HairDepthPeel"] = OpenGLShader({ "GL_hair_depth_peel.vert", "GL_hair_depth_peel.frag" });
        g_shaders["HairFinalComposite"] = OpenGLShader({ "GL_hair_final_composite.comp" });
        g_shaders["HairLayerComposite"] = OpenGLShader({ "GL_hair_layer_composite.comp" });
        g_shaders["HairLighting"] = OpenGLShader({ "GL_hair_lighting.vert", "GL_hair_lighting.frag" });
        g_shaders["HeightMapColor"] = OpenGLShader({ "GL_heightmap_color.vert", "GL_heightmap_color.frag" });
        g_shaders["HeightMapImageGeneration"] = OpenGLShader({ "GL_heightmap_image_generation.comp" });
        g_shaders["HeightMapPhysxTextureGeneration"] = OpenGLShader({ "GL_heightmap_physx_texture_generation.comp" });
        g_shaders["HeightMapVertexGeneration"] = OpenGLShader({ "GL_heightmap_vertex_generation.comp" });
        g_shaders["HeightMapPaint"] = OpenGLShader({ "GL_heightmap_paint.comp" });
        g_shaders["LightCulling"] = OpenGLShader({ "GL_light_culling.comp" });
        g_shaders["Lighting"] = OpenGLShader({ "GL_lighting.comp" });
        g_shaders["CSMLighting"] = OpenGLShader({ "GL_lighting.vert", "GL_lighting.frag"});
        g_shaders["OceanSurfaceComposite"] = OpenGLShader({ "GL_ocean_surface_composite.comp" });
        g_shaders["OceanGeometry"] = OpenGLShader({ "GL_ocean_geometry.vert", "GL_ocean_geometry.frag", "GL_ocean_geometry.tesc", "GL_ocean_geometry.tese" });
        g_shaders["OceanCalculateSpectrum"] = OpenGLShader({ "GL_ocean_calculate_spectrum.comp" });
        g_shaders["OceanUpdateTextures"] = OpenGLShader({ "GL_ocean_update_textures.comp" });
        g_shaders["OceanUnderwaterComposite"] = OpenGLShader({ "GL_ocean_underwater_composite.comp" });
        g_shaders["OceanUnderwaterMaskPreProcess"] = OpenGLShader({ "GL_ocean_underwater_mask_preprocess.comp" });
        g_shaders["OceanPositionReadback"] = OpenGLShader({ "GL_ocean_position_readback.comp" });
        g_shaders["GaussianBlur"] = OpenGLShader({ "GL_gaussian_blur.comp" });
        g_shaders["Outline"] = OpenGLShader({ "GL_outline.vert", "GL_outline.frag" });
        g_shaders["OutlineComposite"] = OpenGLShader({ "GL_outline_composite.comp" });
        g_shaders["OutlineMask"] = OpenGLShader({ "GL_outline_mask.vert", "GL_outline_mask.frag" });
        g_shaders["PointCloudLighting"] = OpenGLShader({ "GL_point_cloud_lighting.comp" });
        g_shaders["PostProcessing"] = OpenGLShader({ "GL_post_processing.comp" });
        g_shaders["ShadowMap"] = OpenGLShader({ "GL_shadow_map.vert", "GL_shadow_map.frag" });
        g_shaders["ShadowCubeMap"] = OpenGLShader({ "GL_shadow_cube_map.vert", "GL_shadow_cube_map.frag" });
        g_shaders["SolidColor"] = OpenGLShader({ "GL_solid_color.vert", "GL_solid_color.frag" });
        g_shaders["Skybox"] = OpenGLShader({ "GL_skybox.vert", "GL_skybox.frag" });
        g_shaders["SpriteSheet"] = OpenGLShader({ "GL_sprite_sheet.vert", "GL_sprite_sheet.frag" });
        g_shaders["UI"] = OpenGLShader({ "GL_ui.vert", "GL_ui.frag" });
        g_shaders["Winston"] = OpenGLShader({ "GL_winston.vert", "GL_winston.frag" });
        g_shaders["CSMDepth"] = OpenGLShader({ "GL_csm_depth.vert", "GL_csm_depth.frag", "GL_csm_depth.geom" });
        g_shaders["ZeroOut"] = OpenGLShader({ "GL_zero_out.comp" });
        g_shaders["VatBlood"] = OpenGLShader({ "GL_vat_blood.vert", "GL_vat_blood.frag" });
    }

    void UpdateSSBOS() {
        const std::vector<GLuint64>& bindlessTextureIDs = OpenGLBackEnd::GetBindlessTextureIDs();
        g_ssbos["Samplers"].Update(bindlessTextureIDs.size() * sizeof(GLuint64), (void*)&bindlessTextureIDs[0]);
        g_ssbos["Samplers"].Bind(0);

        const RendererData& rendererData = RenderDataManager::GetRendererData();
        g_ssbos["RendererData"].Update(sizeof(RendererData), (void*)&rendererData);
        g_ssbos["RendererData"].Bind(1);

        const std::vector<ViewportData>& playerData = RenderDataManager::GetViewportData();
        g_ssbos["ViewportData"].Update(playerData.size() * sizeof(ViewportData), (void*)&playerData[0]);
        g_ssbos["ViewportData"].Bind(2);

        const std::vector<RenderItem>& instanceData = RenderDataManager::GetInstanceData();
        g_ssbos["InstanceData"].Update(instanceData.size() * sizeof(RenderItem), (void*)&instanceData[0]);
        g_ssbos["InstanceData"].Bind(3);

        const std::vector<GPULight>& gpuLightsHighRes = RenderDataManager::GetGPULightsHighRes();
        g_ssbos["Lights"].Update(gpuLightsHighRes.size() * sizeof(GPULight), (void*)&gpuLightsHighRes[0]);
        g_ssbos["Lights"].Bind(4);

        g_ssbos["TileLightData"].Bind(5);

        const std::vector<glm::mat4>& oceanPatchTransforms = RenderDataManager::GetOceanPatchTransforms();
        g_ssbos["OceanPatchTransforms"].Update(oceanPatchTransforms.size() * sizeof(glm::mat4), (void*)&oceanPatchTransforms[0]);

        const std::vector<ScreenSpaceBloodDecalInstanceData>& screenSpaceBloodDecalInstances = RenderDataManager::GetScreenSpaceBloodDecalInstanceData();
        g_ssbos["ScreenSpaceBloodDecals"].Update(screenSpaceBloodDecalInstances.size() * sizeof(ScreenSpaceBloodDecalInstanceData), (void*)&screenSpaceBloodDecalInstances[0]);
        g_ssbos["ScreenSpaceBloodDecals"].Bind(12);
    }

    void PreGameLogicComputePasses() {
        PaintHeightMap();
        ComputeOceanFFTPass();
        OceanHeightReadback();
    }

    void RenderGame() {
        glDisable(GL_DITHER);

        UpdateGlobalIllumintation();
        PointCloudDirectLighting();

        ComputeSkinningPass();
        ClearRenderTargets();

        UpdateSSBOS();
        RenderShadowMaps();
        SkyBoxPass();
        HeightMapPass();
        DecalPaintingPass();
        HouseGeometryPass();
        GrassPass();
        GeometryPass();
        WeatherBoardsPass();
        VatBloodPass();
        ScreenSpaceDecalsPass();
        TextureReadBackPass();
        LightCullingPass();
        LightingPass();
        FurPass();
        OceanGeometryPass();
        OceanSurfaceCompositePass();
        GlassPass();
        DecalPass();
        EmissivePass();
        HairPass();
        OceanUnderwaterCompositePass();
        WinstonPass();
        PostProcessingPass();
        DebugViewPass();
        SpriteSheetPass();
        DebugPass();
        EditorPass();
        OutlinePass();

        DrawPointCloud();

        OpenGLFrameBuffer& gBuffer = g_frameBuffers["GBuffer"];
        OpenGLFrameBuffer& hairFrameBuffer = g_frameBuffers["Hair"];
        OpenGLFrameBuffer& finalImageBuffer = g_frameBuffers["FinalImage"];

        // Downscale blit
        OpenGLRenderer::BlitFrameBuffer(&gBuffer, &finalImageBuffer, "FinalLighting", "Color", GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Blit to swapchain
        OpenGLRenderer::BlitToDefaultFrameBuffer(&finalImageBuffer, "Color", GL_COLOR_BUFFER_BIT, GL_NEAREST);


        UIPass();
        ImGuiPass();

        // World height map
        if (false) {
            OpenGLFrameBuffer& worldFramebuffer = g_frameBuffers["World"];
            BlitRect srcRect;
            srcRect.x0 = 0;
            srcRect.y0 = 0;
            srcRect.x1 = worldFramebuffer.GetWidth();
            srcRect.y1 = worldFramebuffer.GetHeight();

            BlitRect dstRect = srcRect;
            dstRect.x1 = worldFramebuffer.GetWidth() * 2.5f;
            dstRect.y1 = worldFramebuffer.GetHeight() * 2.5f;
            OpenGLRenderer::BlitToDefaultFrameBuffer(&worldFramebuffer, "HeightMap", srcRect, dstRect, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }

        //OpenGLFrameBuffer& fftFramebuffer = g_frameBuffers["FFT"];
        //BlitRect srcRect;
        //srcRect.x0 = 0;
        //srcRect.y0 = 0;
        //srcRect.x1 = fftFramebuffer.GetWidth();
        //srcRect.y1 = fftFramebuffer.GetHeight();
        //
        //BlitRect dstRect = srcRect;
        //dstRect.x1 = BackEnd::GetCurrentWindowHeight();
        //dstRect.y1 = BackEnd::GetCurrentWindowHeight();
        //OpenGLRenderer::BlitToDefaultFrameBuffer(&fftFramebuffer, "Height", srcRect, dstRect, GL_COLOR_BUFFER_BIT, GL_LINEAR);


        //BlitRect srcRect;
        //srcRect.x0 = 0;
        //srcRect.y0 = 0;
        //srcRect.x1 = gBuffer.GetWidth();
        //srcRect.y1 = gBuffer.GetHeight();
        //BlitRect dstRect = srcRect;
        //dstRect.x1 = gBuffer.GetWidth() * 0.6f;
        //dstRect.y1 = gBuffer.GetHeight() * 0.6f;
        //OpenGLRenderer::BlitToDefaultFrameBuffer(&gBuffer, "Glass", srcRect, dstRect, GL_COLOR_BUFFER_BIT, GL_LINEAR);




        // DEBUG RENDER FFT TEXTURES TO THE SCREEN
        static bool showNormals = false;
        if (Input::KeyPressed(HELL_KEY_M)) {
            showNormals = !showNormals;
        }
        if (Input::KeyPressed(HELL_KEY_5)) {
            g_fftDisplayMode = 1;
            g_fftEditBand = 0;
        }
        if (Input::KeyPressed(HELL_KEY_6)) {
            g_fftDisplayMode = 2;
            g_fftEditBand = 1;
        }
        if (Input::KeyPressed(HELL_KEY_7)) {
            g_fftDisplayMode = 0;
        }


        OpenGLFrameBuffer* fft_band0 = &g_frameBuffers["FFT_band0"];
        OpenGLFrameBuffer* fft_band1 = &g_frameBuffers["FFT_band1"];
        OpenGLFrameBuffer* waterFramebuffer = &g_frameBuffers["Water"];

        int size = fft_band0->GetWidth() * 1.5f;

        BlitRect blitRect;
        blitRect.x1 = size;
        blitRect.y1 = size;

        if (GetFftDisplayMode() == 1 && !showNormals) {
            OpenGLRenderer::BlitToDefaultFrameBuffer(fft_band0, "Displacement", blitRect, blitRect, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        if (GetFftDisplayMode() == 1 && showNormals) {
            OpenGLRenderer::BlitToDefaultFrameBuffer(fft_band0, "Normals", blitRect, blitRect, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        if (GetFftDisplayMode() == 2 && !showNormals) {
            OpenGLRenderer::BlitToDefaultFrameBuffer(fft_band1, "Displacement", blitRect, blitRect, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        if (GetFftDisplayMode() == 2 && showNormals) {
            OpenGLRenderer::BlitToDefaultFrameBuffer(fft_band1, "Normals", blitRect, blitRect, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
       // if (GetFftDisplayMode() == 0) {
       //
       //     blitRect.x1 = waterFramebuffer->GetWidth() * 1.0f;// 0.5f;
       //     blitRect.y1 = waterFramebuffer->GetHeight() * 1.0f;//0.5f;
       //     blitRect.x1 = BackEnd::GetFullScreenWidth();
       //     blitRect.y1 = BackEnd::GetFullScreenHeight();
       //     OpenGLRenderer::BlitToDefaultFrameBuffer(waterFramebuffer, "Color", blitRect, blitRect, GL_COLOR_BUFFER_BIT, GL_NEAREST);
       // }
       //
       // std::cout << "GetFftDisplayMode(): " << GetFftDisplayMode() << "\n";


        // Render decal painting shit
        if (false) {
            OpenGLFrameBuffer* decalPaintingFBO = GetFrameBuffer("DecalPainting");
            OpenGLFrameBuffer* decalMasksFBO = GetFrameBuffer("DecalMasks");

            int blitSize = 480;

            glBindFramebuffer(GL_READ_FRAMEBUFFER, decalPaintingFBO->GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glReadBuffer(decalPaintingFBO->GetColorAttachmentSlotByName("UVMap"));
            glDrawBuffer(GL_BACK);
            glBlitFramebuffer(0, 0, decalPaintingFBO->GetWidth(), decalPaintingFBO->GetHeight(), 0, 0, blitSize, blitSize, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, decalMasksFBO->GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glReadBuffer(decalMasksFBO->GetColorAttachmentSlotByName("DecalMask0"));
            glDrawBuffer(GL_BACK);
            glBlitFramebuffer(0, 0, decalMasksFBO->GetWidth(), decalMasksFBO->GetHeight(), 0, blitSize, blitSize, blitSize + blitSize, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }

    void ClearRenderTargets() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* waterFrameBuffer = GetFrameBuffer("Water");
        OpenGLFrameBuffer* finalImageFBO = GetFrameBuffer("FinalImage");
        OpenGLFrameBuffer* miscFullSizeFBO = GetFrameBuffer("MiscFullSize");

        // Water
        waterFrameBuffer->Bind();
        waterFrameBuffer->ClearAttachment("Color", 0, 0, 0, 0);
        waterFrameBuffer->ClearAttachment("UnderwaterMask", 0);
        waterFrameBuffer->ClearAttachment("WorldPosition", 0, 0, 0, 0);

        // GBuffer
        glDepthMask(GL_TRUE);
        gBuffer->Bind();
        gBuffer->ClearAttachment("BaseColor", 0, 0, 0, 0);
        gBuffer->ClearAttachment("Normal", 0, 0, 0, 0);
        gBuffer->ClearAttachment("RMA", 0, 0, 0, 0);
        gBuffer->ClearAttachment("WorldPosition", 0, 0);
        gBuffer->ClearAttachment("Emissive", 0, 0, 0, 0);
        gBuffer->ClearAttachment("Glass", 0, 1, 0, 0);
        gBuffer->ClearDepthAttachment();

        // Decal mask
        miscFullSizeFBO->Bind();
        miscFullSizeFBO->ClearTexImage("ScreenSpaceBloodDecalMask", 0, 0, 0, 0);

        // Viewport index
        for (unsigned int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::ClearFrameBufferByViewportUInt(finalImageFBO, "ViewportIndex", viewport, i);
            }
        }
    }

    void MultiDrawIndirect(const std::vector<DrawIndexedIndirectCommand>& commands) {
        if (commands.size()) {
            // Feed the draw command data to the gpu
            g_indirectBuffer.Bind();
            g_indirectBuffer.Update(sizeof(DrawIndexedIndirectCommand) * commands.size(), commands.data());

            // Fire of the commands
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, (GLsizei)commands.size(), 0);
        }
    }

    void SplitMultiDrawIndirect(OpenGLShader* shader, const std::vector<DrawIndexedIndirectCommand>& commands) {
        const std::vector<RenderItem>& instanceData = RenderDataManager::GetInstanceData();

        for (const DrawIndexedIndirectCommand& command : commands) {
            int viewportIndex = command.baseInstance >> VIEWPORT_INDEX_SHIFT;
            int instanceOffset = command.baseInstance & ((1 << VIEWPORT_INDEX_SHIFT) - 1);

            for (GLuint i = 0; i < command.instanceCount; ++i) {
                const RenderItem& renderItem = instanceData[instanceOffset + i];

                shader->SetInt("u_viewportIndex", viewportIndex);
                shader->SetInt("u_globalInstanceIndex", instanceOffset + i);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalMapTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());

                glDrawElementsBaseVertex(GL_TRIANGLES, command.indexCount, GL_UNSIGNED_INT, (GLvoid*)(command.firstIndex * sizeof(GLuint)), command.baseVertex);
            }
        }
    }


    void HotloadShaders() {
        bool allSucceeded = true;
        for (auto& [_, shader] : g_shaders) {
            if (!shader.Hotload()) {
                allSucceeded = false;
            }
        }
        if (allSucceeded) {
            std::cout << "Hotloaded shaders\n";
        }
    }

    void DrawQuad() {
        static Mesh* mesh = AssetManager::GetMeshByModelNameMeshName("Primitives", "Quad");
        glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
    }

    void CreateBlurBuffers() {
        const Resolutions& resolutions = Config::GetResolutions();

        // Iterate each viewport
        for (int x = 0; x < 4; x++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(x);

            // Start the first blur buffer at the full viewport dimensions
            SpaceCoords spaceCoords = viewport->GetGBufferSpaceCoords();
            float width = resolutions.gBuffer.x;
            float height = resolutions.gBuffer.y;

            // Create framebuffers, downscale by 50% each time
            for (int y = 0; y < 4; y++) {

                // Clean up existing framebuffer
                g_blurBuffers[x][y].Create("BlurBuffer", width, height);
                g_blurBuffers[x][y].CreateAttachment("ColorA", GL_RGBA8);
                g_blurBuffers[x][y].CreateAttachment("ColorB", GL_RGBA8);
                width *= 0.5f;
                height *= 0.5f;
            }
        }
    }

    void CreateSSBO(const std::string& name, size_t size, GLbitfield flags) {
        g_ssbos[name] = OpenGLSSBO(size, flags);
    }

    OpenGLMeshPatch* GetOceanMeshPatch() {
        return &g_tesselationPatch;
    }

    OpenGLShader* GetShader(const std::string& name) {
        auto it = g_shaders.find(name);
        return (it != g_shaders.end()) ? &it->second : nullptr;
    }

    OpenGLFrameBuffer* GetFrameBuffer(const std::string& name) {
        auto it = g_frameBuffers.find(name);
        return (it != g_frameBuffers.end()) ? &it->second : nullptr;
    }

    OpenGLShadowMap* GetShadowMap(const std::string& name) {
        auto it = g_shadowMaps.find(name);
        return (it != g_shadowMaps.end()) ? &it->second : nullptr;
    }

    OpenGLShadowCubeMapArray* GetShadowCubeMapArray(const std::string& name) {
        auto it = g_shadowCubeMapArrays.find(name);
        return (it != g_shadowCubeMapArrays.end()) ? &it->second : nullptr;
    }

    OpenGLShadowMapArray* GetShadowMapArray(const std::string& name) {
        auto it = g_shadowMapArrays.find(name);
        return (it != g_shadowMapArrays.end()) ? &it->second : nullptr;
    }

    OpenGLTextureArray* GetTextureArray(const std::string& name) {
        auto it = g_textureArrays.find(name);
        return (it != g_textureArrays.end()) ? &it->second : nullptr;
    }

    OpenGLFrameBuffer* GetBlurBuffer(int viewportIndex, int bufferIndex) {
        if (viewportIndex < 0 || viewportIndex >= 4) return nullptr;
        if (bufferIndex < 0 || bufferIndex >= 4) return nullptr;
        return &g_blurBuffers[viewportIndex][bufferIndex];
    }

    OpenGLSSBO* GetSSBO(const std::string& name) {
        auto it = g_ssbos.find(name);
        return (it != g_ssbos.end()) ? &it->second : nullptr;
    }

    OpenGLCubemapView* GetCubemapView(const std::string& name) {
        auto it = g_cubemapViews.find(name);
        return (it != g_cubemapViews.end()) ? &it->second : nullptr;
    }

    OpenGLRasterizerState* GetRasterizerState(const std::string& name) {
        auto it = g_rasterizerStates.find(name);
        return (it != g_rasterizerStates.end()) ? &it->second : nullptr;
    }

    OpenGLRasterizerState* CreateRasterizerState(const std::string& name) {
        g_rasterizerStates[name] = OpenGLRasterizerState();
        return &g_rasterizerStates[name];
    }

    void SetRasterizerState(const std::string& name) {
        OpenGLRasterizerState* rasterizerState = GetRasterizerState(name);
        if (!rasterizerState) {
            std::cout << "OpenGLRenderer::SetRasterizerState(const std::string& name) failed! " << name << " does not exist!\n";
            return;
        }

        rasterizerState->blendEnable ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        rasterizerState->cullfaceEnable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        rasterizerState->depthMask ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
        rasterizerState->depthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

        if (rasterizerState->blendEnable) {
            glBlendFunc(rasterizerState->blendFuncSrcfactor, rasterizerState->blendFuncDstfactor);
        }
        if (rasterizerState->depthTestEnabled) {
            glDepthFunc(rasterizerState->depthFunc);
        }
        if (rasterizerState->pointSize > 1.0f) {
            glPointSize(rasterizerState->pointSize);
        }
    }

    std::vector<float>& GetShadowCascadeLevels() {
        return g_shadowCascadeLevels;
    }

    void UpdateSSBO(const std::string& name, size_t size, const void* data) {
        OpenGLSSBO* ssbo = GetSSBO(name);
        if (ssbo && size > 0) {
            ssbo->Update(size, data);
        }
    }
}
























//if (rasterizerState->depthTestEnabled) {
//    glEnable(GL_DEPTH_TEST);
//}
//else {
//    glDisable(GL_DEPTH_TEST);
//}
//