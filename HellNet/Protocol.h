#pragma once
#include <cstdint>
#include "NetConfig.h"

namespace hellnet {

enum class Msg : uint8_t {
    JoinRequest=1, JoinAccept=2, InputFrame=3, Snapshot=4, Ping=5, Pong=6, FireEvent=7
};

#pragma pack(push, 1)
struct NetHeader {
    uint8_t  magic   = 0xAB;
    uint8_t  version = 1;
    Msg      msg;
    uint32_t seq = 0;
    uint32_t ack = 0;
};
#pragma pack(pop)

// Plain wire structs for PoC (packing can be enabled later via kEnableWirePacking)
struct InputFrame {
    uint32_t tick;
    float    yaw, pitch;
    int8_t   moveX;
    int8_t   moveY;
    uint16_t buttons;
};

struct PlayerState {
    uint32_t tick;
    float    px, py, pz;
    float    vx, vy, vz;
    float    yaw, pitch;
    uint16_t ammo;
    uint8_t  health;
};

// Host sends both players. Client uses the "client" field as "you".
struct Snapshot {
    uint32_t   tick;
    PlayerState host;
    PlayerState client;
};

struct FireEvent {
    uint32_t tick;
    float    origin[3];
    float    dir[3];
    uint32_t seqId;
    uint8_t  shooterIsHost;
};

} // namespace hellnet
