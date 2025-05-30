#pragma once
#include "Types/Renderer/MeshBuffer.h"
#include <vector>

namespace AStarMap {
    void Init();
    void Update();
    void UpdateDebugMeshesFromHeightField();
    void MarkCellAsObstacle(int x, int y);
    bool IsInBounds(int x, int y);
    bool IsCellObstacle(int x, int y);

    std::vector<glm::ivec2> GetWallCells();
    int GetMapWidth();
    int GetMapHeight();
    int GetCellCount();
    glm::ivec2 GetCellCoordsFromWorldSpacePosition(glm::vec3 position);

    MeshBuffer& GetDebugGridMeshBuffer(); 
    MeshBuffer& GetDebugSolidMeshBuffer();
}