#include "Game.h"
#include "HellTypes.h"

#include "../Renderer/Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Backend/Backend.h"
#include "Audio/Audio.h"
#include "Core/JSON.h"
#include "Editor/Editor.h"
#include "Imgui/ImGuiBackEnd.h"
#include "File/File.h"
#include "Input/Input.h"
#include "Player/Player.h"
#include "Physics/Physics.h"
#include "Tools/ImageTools.h"
#include "UI/UIBackEnd.h"
#include "Viewport/ViewportManager.h"

// Get me out of here
##include "../../../HellNet/NetSystem.h"
n#clude "World/World.h"
// Get me out of here

namespace Game {
    float g_deltaTime = 0;
    float g_totalTime = 0;
    double g_deltaTimeAccumulator = 0.0;
    double g_fixedDeltaTime = 1.0 / 60.0;
    glm::vec3 g_moonlightDirection = glm::normalize(glm::vec3(0.0, 0.2, 0.5));
    std::vector<Player> g_localPlayers;
    std::vector<Player> g_onlinePlayers;
    hellnet::NetSystem gNet;
    
uint32_t g_netTick = 0;

// --- Netcode integration (P2P 1v1) ---
// Latest snapshot from host (for clients)
static hellnet::Snapshot g_latestSnapshot;
// Callback when host receives remote input (host-only)
static void Net_OnHostReceiveRemoteInput(const hellnet::InputFrame& in);
// Callback when client receives authoritative snapshot (client-only)
static void Net_OnClientApplySnapshot(const hellnet::Snapshot& snap);
// Callback when host receives a fire event (host-only)
static void Net_OnHostFireEvent(const hellnet::FireEvent& fe);

void Create()
 AddLocalPlayer(
     NetSystem gNet
       SplitscreenMode g_splitscreenMode = SplitscreenMode::FULLSCREEN;

    void UpdateAudioLoops();

    void AddLocalPlayer(glm::vec3 position, glm::vec3 rotation) {
        if (g_localPlayers.size() == 4) {
            return;
        }
        Player& player = g_localPlayers.emplace_back();
        player.Init(position, rotation, g_localPlayers.size() - 1);
    }

    void AddOnlinePlayer(glm::vec3 position, glm::vec3 rotation) {
        Player& player = g_onlinePlayers.emplace_back();
        player.Init(position, rotation, -1);
    }

    void Create() {

        //SectorCreateInfo sectorCreateInfo = JSON::LoadSector("res/sectors/TestSector.json");
        //World::LoadSingleSector(sectorCreateInfo);

        //

        // Create players
        AddLocalPlayer(glm::vec3(12.82, 0.5f, 18.27f), glm::vec3(-0.13f, -1.46f, 0.0f));
        AddLocalPlayer(glm::vec3(15.21, 0.5f, 19.57), glm::vec3(-0.49f, -0.74f, 0.0f));
        //AddLocalPlayer(glm::vec3(10, 0.5f, 22), glm::vec3(-0.30f, -4.5f, 0.0f));
        //AddLocalPlayer(glm::vec3(12, 0.5f, 22), glm::vec3(-0.30f, -4.5f, 0.0f));

        SetPlayerKeyboardAndMouseIndex(0, 0, 0);
        SetPlayerKeyboardAndMouseIndex(1, 1, 1);
        //SetPlayerKeyboardAndMouseIndex(2, 1, 1);
        //SetPlayerKeyboardAndMouseIndex(3, 1, 1);

        SetSplitscreenMode(SplitscreenMode::FULLSCREEN);
       
        // Netcode: initialize P2P host
    {
        hellnet::NetCallbacks netCallbacks{};
        netCallbacks.onHostReceiveRemoteInput = &Net_OnHostReceiveRemoteInput;
       
        

        netCallbacks.onClientApplySnapshot = &Net_OnClientApplySnapshot;
        netCallbacks.onHostFireEvent = &Net_OnHostFireEvent;
        gNet.setCallbacks(netCallbacks);
        gNet.host();
    }

        //SetSplitscreenMode(SplitscreenMode::TWO_PLAYER);

        Audio::PlayAudio("Glock_Equip.wav", 0.5f);
    }

    void BeginFrame() {
        for (Player& player : g_localPlayers) {
            player.BeginFrame();
        }
    }

    void Update() {
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        g_deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        g_deltaTimeAccumulator += g_deltaTime;
                // Netcode: pump network messages
        gNet.pump(currentTime * 1000.0);


        
        // Total time
        g_totalTime += g_deltaTime;
        if (g_totalTime > TIME_WRAP) {
            g_totalTime -= TIME_WRAP; // Keep it continuous
        }

        World::UpdatePlayerBvhs();

        // Editor select menu open?
        if (Editor::IsOpen() || ImGuiBackEnd::OwnsMouse()) {
            for (Player& player : g_localPlayers) {
                player.DisableControl();
            }
        } 
        else {
            for (Player& player : g_localPlayers) {
                player.EnableControl();
            }
        }

        for (Player& player : g_localPlayers) {
            player.Update(g_deltaTime);
        }

        World::Update(g_deltaTime);

        // Physics
        while (g_deltaTimeAccumulator >= g_fixedDeltaTime) {
            g_deltaTimeAccumulator -= g_fixedDeltaTime;
                        // Netcode: capture local input and send
            if (!g_localPlayers.empty()) {
                hellnet::InputFrame input{};
                input.tick = g_netTick;
                // assume first local player is the controlling player
                Player& p = g_localPlayers[0];

                glm::vec3 camRot = p.GetCameraRotation();
                input.yaw = camRot.y;
                input.pitch = camRot.x;
                int8_t moveX = 0;
                if (p.PressingWalkLeft()) moveX -= 127;
                if (p.PressingWalkRight()) moveX += 127;
                input.moveX = moveX;
                int8_t moveY = 0;
                if (p.PressingWalkBackward()) moveY -= 127;
                if (p.PressingWalkForward()) moveY += 127;
                input.moveY = moveY;
                uint16_t buttons = 0;
                if (p.PressingJump()) buttons |= 1;
                if (p.PressingCrouch()) buttons |= 2;
                if (p.PressingFire()) buttons |= 4;
                if (p.PressedReload()) buttons |= 8;
                input.buttons = buttons;
                gNet.captureLocalInput(input);
            }

            // If host, build authoritative snapshot and send
            if (gNet.role() == hellnet::Role::Host) {
                hellnet::PlayerState hostState{};
                hellnet::PlayerState clientState{};
                // Fill host state from first local player
                if (!g_localPlayers.empty()) {
                    Player& hp = g_localPlayers[0];
                    hostState.tick = g_netTick;
                    glm::vec3 pos = hp.GetCameraPosition();
                    hostState.px = pos.x;
 
                    hostState.py = pos.y;

                    hostState.pz = pos.z;
                    hostState.vx = 0.0f;
                    hostState.vy = 0.0f;
                    hostState.vz = 0.0f;
                    glm::vec3 hrot = hp.GetCameraRotation();
                    hostState.yaw = hrot.y;
                    hostState.pitch = hrot.x;
                    hostState.ammo = (uint16_t)hp.GetCurrentWeaponMagAmmo();
                    hostState.health = 100;
                }
                // Fill client state from first online player (if any)
                if (!g_onLinePlayers.empty()) {
                    Player& cp = g_onLinePlayers[0];
                    clientState.tick = g_netTick;
                    glm::vec3 pos = cp.GetCameraPosition();
                    clientState.px = pos.x;
                    clientState.py = pos.y;
                    clientState.pz = pos.z;
                    clientState.vx = 0.0f;
                    clientState.vy = 0.0f;
                    clientState.vz = 0.0f;
                    glm::vec3 crot = cp.GetCameraRotation();
                    clientState.yaw = crot.y;
                    clientState.pitch = crot.x;
                    clientState.ammo = (uint16_t)cp.GetCurrentWeaponMagAmmo();
                    clientState.health = 100;
                }
                gNet.hostSubmitAuthoritativeStates(g_netTick, hostState, clientState);
            }

                // Advance net tick
                            ++g_netTick;
    
            if (Editor::IsClosed()) {
                Physics::StepPhysics(g_fixedDeltaTime);
            }
        }

        UpdateAudioLoops();
    }

    float GetDeltaTime() {
        return g_deltaTime;
    }

    float GetTotalTime() {
        return g_totalTime;
    }

    void RespawnPlayers() {
        for (Player& player : g_localPlayers) {
            player.Respawn();
        }
    }

    Player* GetPlayerByPlayerId(uint64_t playerId) {
        for (Player& player : g_localPlayers) {
            if (player.GetPlayerId() == playerId) {
                return &player;
            }
        }
        return nullptr;
    }

    Player* GetLocalPlayerByIndex(uint32_t index) {
        if (index >= 0 && index < g_localPlayers.size()) {
            return &g_localPlayers[index];
        }
        else {
            std::cout << "Game::GetPlayerByIndex(int index) failed. " << index << " out of range of size " << g_localPlayers.size() << "\n";
            return nullptr;
        }
    }

    Camera* GetLocalPlayerCameraByIndex(uint32_t index) {
        if (index >= 0 && index < g_localPlayers.size()) {
            return &g_localPlayers[index].GetCamera();
        }
        else {
            std::cout << "Game::GetLocalPlayerCameraByIndex(int index) failed. " << index << " out of range of local player count " << g_localPlayers.size() << "\n";
            return nullptr;
        }
    }

    float GetLocalPlayerFovByIndex(uint32_t index) {
        if (index >= 0 && index < g_localPlayers.size()) {
            return g_localPlayers[index].GetFov();
        }
        else {
            std::cout << "Game::GetLocalPlayerFOVByIndex(int index) failed. " << index << " out of range of local player count " << g_localPlayers.size() << "\n";
            return 1.0f;
        }
    }

    void NextSplitScreenMode() {
        int nextSplitscreenMode = ((int)(g_splitscreenMode) + 1) % ((int)(SplitscreenMode::SPLITSCREEN_MODE_COUNT));
        SetSplitscreenMode((SplitscreenMode)nextSplitscreenMode);
        std::cout << "Splitscreen mode: " << nextSplitscreenMode << "\n";
    }

    void SetSplitscreenMode(SplitscreenMode mode) {
        g_splitscreenMode = mode;
        //ViewportManager::UpdateViewports();
    }

    const SplitscreenMode& GetSplitscreenMode() {
        return g_splitscreenMode;
    }

    int32_t GetLocalPlayerCount() {
        return g_localPlayers.size();
    }

    int32_t GetOnlinePlayerCount() {
        return g_onlinePlayers.size();
    }

    int32_t GetActiveViewportCount() {
        if (g_splitscreenMode == SplitscreenMode::FULLSCREEN) {
            return std::min(GetLocalPlayerCount(), (int32_t)1);
        }
        else if (g_splitscreenMode == SplitscreenMode::TWO_PLAYER) {
            return std::min(GetLocalPlayerCount(), (int32_t)2);
        }
        else if (g_splitscreenMode == SplitscreenMode::FOUR_PLAYER) {
            return std::min(GetLocalPlayerCount(), (int32_t)4);
        } 
        else{
            return 1;
        }
    }

    void SetPlayerKeyboardAndMouseIndex(int playerIndex, int keyboardIndex, int mouseIndex) {
        if (playerIndex >= 0 && playerIndex < g_localPlayers.size()) {
            g_localPlayers[playerIndex].SetKeyboardIndex(keyboardIndex);
            g_localPlayers[playerIndex].SetMouseIndex(mouseIndex);
        }
    }


    void PlayFootstepIndoorAudio() {
        const std::vector<const char*> indoorFootstepFilenames = {
                    "player_step_1.wav",
                    "player_step_2.wav",
                    "player_step_3.wav",
                    "player_step_4.wav",
        };
        int random = rand() % 4;
        Audio::PlayAudio(indoorFootstepFilenames[random], 0.5f);
    }

    void PlayFootstepOutdoorAudio() {
        const std::vector<const char*> indoorFootstepFilenames = {
                "player_step_grass_1.wav",
                "player_step_grass_2.wav",
                "player_step_grass_3.wav",
                "player_step_grass_4.wav",
        };
        int random = rand() % 4;
        Audio::PlayAudio(indoorFootstepFilenames[random], 0.5f);
    }

    void UpdateAudioLoops() {
        // Under water loop
        bool playersUnderWater = false;
        for (Player& player : g_localPlayers) {
            if (player.CameraIsUnderwater() && player.ViewportIsVisible() && player.IsAlive()) {
                playersUnderWater = true;
                break;
            }
        }
        if (playersUnderWater && g_totalTime > 1.0f) {
            Audio::LoopAudioIfNotPlaying("Water_AmbientLoop.wav", 1.0);
        }
        else {
            Audio::StopAudio("Water_AmbientLoop.wav");
        }

        // Wading loop
        Player& player = g_localPlayers[0]; // WARNNG!!! Only works for player 0
        if (player.IsWading()) {
            Audio::LoopAudioIfNotPlaying("Water_PaddlingLoop_1.wav", 1.0);
        }
        else {
            Audio::StopAudio("Water_PaddlingLoop_1.wav");
        }
    }

    glm::vec3 GetMoonlightDirection() {
        return g_moonlightDirection;
    }
}


namespace Game {

static void Net_OnHostReceiveRemoteInput(const hellnet::InputFrame& in) {
    // TODO: handle remote input on host
}

static void Net_OnClientApplySnapshot(const hellnet::Snapshot& snap) {
    g_latestSnapshot = snap;
    // TODO: apply snapshot to update client state
}

static void Net_OnHostFireEvent(const hellnet::FireEvent& fe) {
    // TODO: handle fire event on host (lag compensation)
}

} // namespace Game
