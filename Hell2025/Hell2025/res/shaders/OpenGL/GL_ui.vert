#version 460 core

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec4 vColor;

out vec4 Color;
out vec2 TexCoord;

// Make these NOT uniforms
uniform int u_clipMinX;
uniform int u_clipMinY;
uniform int u_clipMaxX;
uniform int u_clipMaxY;

void main() {
    Color = vColor;
    TexCoord = vUV;

    vec4 clip = vec4(vPosition, 0.0, 1.0);
    gl_Position = clip;

    vec2 uViewportSizePx = vec2(1920, 1080);
    vec2 uRectMinPx = vec2(u_clipMinX, u_clipMinY);
    vec2 uRectMaxPx = vec2(u_clipMaxX, u_clipMaxY);
    
    vec2 ndcMin = vec2((uRectMinPx.x / uViewportSizePx.x) * 2.0 - 1.0,
                       (uRectMinPx.y / uViewportSizePx.y) * 2.0 - 1.0);
    vec2 ndcMax = vec2((uRectMaxPx.x / uViewportSizePx.x) * 2.0 - 1.0,
                       (uRectMaxPx.y / uViewportSizePx.y) * 2.0 - 1.0);
    
    vec2 ndc = clip.xy / clip.w;
    
    //gl_ClipDistance[0] = ndc.x - ndcMin.x;
    //gl_ClipDistance[1] = ndcMax.x - ndc.x;
    //gl_ClipDistance[2] = ndc.y - ndcMin.y;
    gl_ClipDistance[3] = ndcMax.y - ndc.y;
}
