#version 460 core
#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"
#include "../common/types.glsl"
#include "../common/util.glsl"

layout (binding = 0) uniform sampler2D BaseColorTexture;
layout (binding = 1) uniform sampler2D NormalTexture;
layout (binding = 2) uniform sampler2D RMATexture;
layout (binding = 3) uniform sampler2D BlueNoiseTexture;
layout (binding = 4) uniform samplerCubeArray highResShadowCubeMapArray;
layout (binding = 5) uniform sampler2D FlashlightCookieTexture;
layout (binding = 6) uniform sampler2DArray FlashlighShadowMapArrayTexture;
layout (binding = 7) uniform sampler2D FurMaskTexture;

layout (location = 0) out vec4 FinalLightingOut;

readonly restrict layout(std430, binding = 1) buffer rendererDataBuffer { RendererData  rendererData;   };
readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer { ViewportData  viewportData[]; };
readonly restrict layout(std430, binding = 4) buffer lightsBuffer       { Light         lights[];       };
readonly restrict layout(std430, binding = 5) buffer tileDataBuffer     { TileLightData tileData[];     };

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;

uniform int u_viewportIndex;

in float H0;

void main() {
    vec2 uv = TexCoord * 20.0; //bunny
    //vec2 uv = TexCoord * 25.0;
    const float MAX_LOD = 2.0;

    float furMask = texture(BaseColorTexture, TexCoord).r;

    float wantLod = textureQueryLod(BlueNoiseTexture, uv).x;
    float lod = clamp(wantLod, 0.0, MAX_LOD);
    float blueNoise = textureLod(BlueNoiseTexture, uv, lod).r;

    if (blueNoise < 0.25) {
       discard;
    }

    float alpha = (1 - H0 * 1) * blueNoise;
    alpha = clamp(alpha, 0, 1);
    if (alpha < 0.05) {
       discard;
    }
    
    vec3 viewPos = viewportData[u_viewportIndex].viewPos.xyz;

    vec4 baseColor = texture(BaseColorTexture, TexCoord);
    vec3 gammaBaseColor = pow(baseColor.rgb, vec3(2.2));
    vec4 normalMap = texture(NormalTexture, TexCoord) * 2.0 - 1.0;
    mat3 TBN = mat3(normalize(Tangent), normalize(BiTangent), normalize(Normal));
    vec3 normal = normalize(TBN * normalMap.rgb);

    vec4 rma = texture(RMATexture, TexCoord);
    float roughness = rma.r;
    float metallic = rma.g;

    ivec2 tile = ivec2(gl_FragCoord.xy) / TILE_SIZE;
    uint tileIndex = uint(tile.y) * rendererData.tileCountX + uint(tile.x);
    uint lightCount = tileData[tileIndex].lightCount;

    vec3 directLighting = vec3(0.0);
    for (uint i = 0; i < lightCount; ++i) {
        uint lightIndex = tileData[tileIndex].lightIndices[i];
        Light light = lights[lightIndex];
        vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
        vec3 lightColor = vec3(light.colorR, light.colorG, light.colorB);
        float shadow = ShadowCalculationFast(int(lightIndex), lightPosition, light.radius, WorldPos.xyz, viewPos, normal, highResShadowCubeMapArray);
        directLighting += GetDirectLighting(lightPosition, lightColor, light.radius, light.strength, normal, WorldPos.xyz, gammaBaseColor, roughness, metallic, viewPos) * shadow;
    }

    // Flashlights
    for (int i = 0; i < 1; i++) {
        float flashlightModifer = viewportData[i].flashlightModifer;
        if (flashlightModifer > 0.05) {
            mat4 flashlightProjectionView = viewportData[i].flashlightProjectionView;
            vec4 flashlightDir = viewportData[i].flashlightDir;
            vec4 flashlightPosition = viewportData[i].flashlightPosition;
            vec3 flashlightViewPos = viewportData[i].inverseView[3].xyz;
            vec3 spotLightPos = flashlightPosition.xyz;
            vec3 spotLightDir = flashlightDir.xyz;
            vec3 spotLightColor = vec3(0.9, 0.95, 1.1);
            float innerAngle = cos(radians(5.0 * flashlightModifer));
            float outerAngle = cos(radians(25.0));
            vec3 cookie = ApplyCookie(flashlightProjectionView, WorldPos.xyz, spotLightPos, spotLightColor, 10, FlashlightCookieTexture);
            float shadow = 1 - SpotlightShadowCalculationFast(flashlightProjectionView * WorldPos, normal, spotLightDir, WorldPos.xyz, spotLightPos, flashlightViewPos, FlashlighShadowMapArrayTexture, i); 
            vec3 spotLighting = GetSpotlightLighting(spotLightPos, spotLightDir, spotLightColor, 50.0, 3.0, innerAngle, outerAngle, normal, WorldPos.xyz, gammaBaseColor, roughness, metallic, flashlightViewPos, flashlightProjectionView) * shadow;
            directLighting += spotLighting * flashlightModifer * cookie * spotLightColor;
        }
    }

    directLighting = clamp(directLighting, 0, 1);

    // Ambient light
    vec3 amibentLightColor = vec3(1, 0.98, 0.94);
    float ambientIntensity = 0.0025;
    vec3 ambientColor = baseColor.rgb * amibentLightColor;
    vec3 ambientLighting = ambientColor * ambientIntensity;

    // Ambient hack
	float factor = min(1, 1 - metallic * 1.0);
	ambientLighting *= (1.0) * vec3(factor);

    // composite PBR
    vec3 finalColor = directLighting + ambientLighting;

    finalColor = clamp(finalColor, 0, 1);

    alpha*= furMask;
    
    FinalLightingOut = vec4(finalColor, alpha * 2);  // 2 is bunny, not sure about roo 
    //FinalLightingOut = vec4(vec3(furMask), 1);



}
