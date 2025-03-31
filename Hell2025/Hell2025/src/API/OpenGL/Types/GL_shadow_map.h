#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct OpenGLShadowMap {

public:
    OpenGLShadowMap() = default;
    OpenGLShadowMap(const std::string& name, int width, int height, int layers);
    void CleanUp();
    void Bind();
    void Unbind();
    void BindLayer(int layerIndex);
    void ClearLayer(int layerIndex);
    void Resize(int width, int height, int layers = 1);
    //void ClearDepth();
    void SetViewport() const;
    GLuint GetDepthTextureHandle() const;
    GLuint GetFramebuffer() const;

private:
    std::string m_name;
    GLuint m_handle = 0;
    GLuint m_depthTexture = 0;
    GLuint m_width = 0;
    GLuint m_height = 0;
    GLuint m_layers = 1;
};
