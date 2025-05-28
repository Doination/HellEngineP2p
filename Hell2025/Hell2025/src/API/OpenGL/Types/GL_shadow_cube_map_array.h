#pragma once

struct OpenGLShadowCubeMapArray {
    void Init(unsigned int numberOfCubemaps, int size);
    void CleanUp();
    void ClearDepthLayers(float clearValue = 1.0f);
    void ClearDepthLayer(int layerIndex, float clearValue = 1.0f);
    unsigned int GetHandle()        const { return m_handle; }
    unsigned int GetSize()          const { return m_size; }
    unsigned int GetDepthTexture()  const { return m_depthTexture; }
    unsigned int GetLayerCount()    const { return m_numberOfCubemaps; }
    unsigned int GetTextureView()   const { return m_textureView; }

private:
    unsigned int m_size = 0;
    unsigned int m_handle = 0;
    unsigned int m_depthTexture = 0;
    unsigned int m_numberOfCubemaps = 0;
    unsigned int m_textureView = 0;
};