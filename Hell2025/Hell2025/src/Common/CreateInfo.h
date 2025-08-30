#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include "HellDefines.h"
#include <map>

struct ToiletCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
};

struct GenericStaticCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    GenericStaticType type = GenericStaticType::COUCH;
};

struct GenericBouncableCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    GenericBouncableType type = GenericBouncableType::COUCH_CUSHION_0;
};

/*
struct MeleeBulletWaveCreateInfo {
    glm::vec3 startPos = glm::vec3(0.0f);
    glm::vec3 localXYPlaneDir = glm::vec3(0.0f);
    float startTime = 0.0f;
    float maxTime = 0.0f;
    float intervalDuration = 0.0f;;
};*/

struct ChristmasLightsCreateInfo {
    // Hanging lights
    glm::vec3 start = glm::vec3(0);
    glm::vec3 end = glm::vec3(0);
    float sag = 0;

    // Tree spiral lights
    bool spiral = false;
    float spiralRadius = 1.0f;
    float spiarlHeight = 1.0f;
    glm::vec3 sprialTopCenter;
};

struct ChristmasPresentCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    ChristmasPresentType type = ChristmasPresentType::SMALL;
};

struct ChristmasTreeCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
};

struct DecalCreateInfo {
    DecalType decalType;
    PhysicsType parentPhysicsType = PhysicsType::UNDEFINED;
    ObjectType parentObjectType = ObjectType::UNDEFINED;
    glm::vec3 surfaceHitPosition = glm::vec3(0.0f);
    glm::vec3 surfaceHitNormal = glm::vec3(0.0f);
    uint64_t parentPhysicsId = 0;
    uint64_t parentObjectId = 0;
};

struct ScreenSpaceBloodDecalCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    int type = 0;
};

struct MermaidCreateInfo {
    glm::vec3 position;
    glm::vec3 rotation;
};

struct KangarooCreateInfo {
    glm::vec3 position;
    glm::vec3 rotation;
};

struct DoorCreateInfo {
    glm::vec3 position;
    glm::vec3 rotation;
};

struct WindowCreateInfo {
    glm::vec3 position;
    glm::vec3 rotation;
};

struct PlaneCreateInfo {
    glm::vec3 p0 = glm::vec3(0.0f);
    glm::vec3 p1 = glm::vec3(0.0f);
    glm::vec3 p2 = glm::vec3(0.0f);
    glm::vec3 p3 = glm::vec3(0.0f);
    std::string materialName = "";
    float textureScale = 1.0f;
    float textureOffsetU = 0.0f;
    float textureOffsetV = 0.0f;
    float textureRotation = 0.0f;
};

struct PianoCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
};

struct WallCreateInfo {
    std::vector<glm::vec3> points;
    std::string materialName = "";
    float height = 2.4f;
    float textureScale = 1.0f;
    float textureOffsetU = 0.0f;
    float textureOffsetV = 0.0f;
    float textureRotation = 0.0f;
    bool useReversePointOrder = false;
    TrimType ceilingTrimType = TrimType::NONE;
    TrimType floorTrimType = TrimType::NONE;
    WallType wallType = WallType::INTERIOR;
};

struct LightCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1, 0.7799999713897705, 0.5289999842643738);
    float radius = 6.0f;
    float strength = 1.0f;
    LightType type = LightType::HANGING_LIGHT;
};

struct BulletCreateInfo {
    glm::vec3 origin = glm::vec3(0);
    glm::vec3 direction = glm::vec3(0);
    int32_t weaponIndex = 0;
    uint32_t damage = 0;
    uint32_t ownerObjectId = 0;
    float rayLength = 1000.0f;
    bool createsDecals = true;
    bool createsFollowThroughBulletOnGlassHit = true;
    bool playsPiano = true;
    bool createsDecalTexturePaintedWounds = true;
};

struct BasicDoorCreateInfo {
    glm::mat4 parentMatrix = glm::mat4(1.0f);
    Axis rotationAxis = Axis::NONE;
    std::vector<MeshRenderingInfo> m_meshRenderingInfoList;
};

struct PickUpCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 intitialForce = glm::vec3(0.0f);
    std::string pickUpType = "";
};

struct PictureFrameCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    PictureFrameType type = PictureFrameType::BIG_LANDSCAPE;
};

struct SpriteSheetObjectCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool loop = false;
    bool billboard = true;
    bool renderingEnabled = true;
    float animationSpeed = 1.0f;
    std::string textureName = "";
};

struct BulletCasingCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 force = glm::vec3(0.0f);
    float mass = 0.0f;
    uint32_t modelIndex = 0;
    uint32_t materialIndex = 0;
};

struct GameObjectCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    std::string modelName = "";
    std::vector<MeshRenderingInfo> meshRenderingInfoSet;
};

struct TreeCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    TreeType type = TreeType::TREE_LARGE_0;
};

struct SectorCreateInfo {
    std::string sectorName;
    std::string heightMapName;
    std::vector<GameObjectCreateInfo> gameObjects;
    std::vector<LightCreateInfo> lights;
    std::vector<PickUpCreateInfo> pickUps;
    std::vector<TreeCreateInfo> trees;

    // TO DO:
    //std::vector<InteractTextCreateInfo> interactTexts;
    //std::vector<HouseLocation> houseLocations;
    //glm::vec3 mermaidsLocation;
};

struct HouseCreateInfo {
    std::vector<DoorCreateInfo> doors;
    std::vector<PlaneCreateInfo> planes;
    std::vector<LightCreateInfo> lights;
    std::vector<PianoCreateInfo> pianos;
    std::vector<PictureFrameCreateInfo> pictureFrames;
    std::vector<WallCreateInfo> walls;
    std::vector<WindowCreateInfo> windows;
};

struct MapCreateInfo {
    std::string name;
    uint32_t width = 4;
    uint32_t depth = 4;
    ivecXZ spawnCoords = ivecXZ(0, 0);
    std::string m_sectorNames[MAX_MAP_WIDTH][MAX_MAP_DEPTH];
};