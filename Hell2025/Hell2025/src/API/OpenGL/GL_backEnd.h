#pragma once
#include "HellEnums.h"
#include "API/OpenGL/Types/GL_heightmap_mesh.h"
#include "Types/GL_texture.h"
#include "Types/Renderer/Texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

namespace OpenGLBackEnd {
    // Core
    void Init();
    void BeginFrame();

    // Textures
    void UpdateTextureBaking();
    void AllocateTextureMemory(Texture& texture);
    void ImmediateBake(QueuedTextureBake& queuedTextureBake);
    void AsyncBakeQueuedTextureBake(QueuedTextureBake& queuedTextureBake);
    void CleanUpBakingPBOs();
    const std::vector<GLuint64>& GetBindlessTextureIDs();

    // Buffers
    void UploadVertexData(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void UploadWeightedVertexData(std::vector<WeightedVertex>& vertices, std::vector<uint32_t>& indices);
    void AllocateSkinnedVertexBufferSpace(int vertexCount);

    OpenGLHeightMapMesh& GetHeightMapMesh();
    void ReadBackHeightmapMeshData();

    GLuint GetVertexDataVAO();
    GLuint GetVertexDataVBO();
    GLuint GetVertexDataEBO();
    GLuint GetWeightedVertexDataVAO();
    GLuint GetWeightedVertexDataVBO();
    GLuint GetWeightedVertexDataEBO();
    GLuint GetSkinnedVertexDataVAO();
    GLuint GetSkinnedVertexDataVBO();
}