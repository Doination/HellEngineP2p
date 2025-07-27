#include "World.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Pathfinding/AStarMap.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"

namespace World {

    void LazyDebugSpawns();
    void CalculateGPULights();

    // REMOVE ME!
   //uint64_t g_rooAnimatedGameObject = 0;
   //AnimatedGameObject* GetRooTest() {
   //    return GetAnimatedGameObjectByObjectId(g_rooAnimatedGameObject);
   //}
    // 



    void Update(float deltaTime) {

        /*
        {
        // Visualize dot product arc
            glm::vec3 forward = Game::GetLocalPlayerByIndex(0)->GetCameraForward();
            forward.y = 0.0f;
            forward = glm::normalize(forward);
            glm::vec3 origin = Game::GetLocalPlayerByIndex(0)->GetFootPosition();
            float dotThreshold = 0.7f;
            float angle = acos(dotThreshold);
            glm::mat4 rotRight = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0, 1, 0));
            glm::mat4 rotLeft = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            glm::vec3 rightEdge = glm::vec3(rotRight * glm::vec4(forward, 0.0f));
            glm::vec3 leftEdge = glm::vec3(rotLeft * glm::vec4(forward, 0.0f));
            float length = 2.0f;
            glm::vec4 color = RED;

            AnimatedGameObject* roo = GetAnimatedGameObjectByObjectId(g_rooAnimatedGameObject);
            glm::vec3 target = roo->_transform.position;
            glm::vec3 toTarget = glm::normalize(target - origin);
            float dotValue = glm::dot(forward, toTarget);
            if (dotValue >= dotThreshold) {
                color = GREEN;
            }
            Renderer::DrawLine(origin, origin + forward * length, color);     // center
            Renderer::DrawLine(origin, origin + leftEdge * length, color);    // left limit
            Renderer::DrawLine(origin, origin + rightEdge * length, color);   // right limit
        }*/

        

        ProcessBullets();
        LazyDebugSpawns();

        std::vector<AnimatedGameObject>& animatedGameObjects = GetAnimatedGameObjects();
        std::vector<BulletCasing>& bulletCasings = GetBulletCasings();
        std::vector<Bullet>& bullets = GetBullets();
        std::vector<Door>& doors = GetDoors();
        std::vector<GameObject>& gameObjects = GetGameObjects();
        std::vector<Light>& lights = GetLights();
        std::vector<Piano>& pianos = GetPianos();
        std::vector<PickUp>& pickUps = GetPickUps();
        std::vector<Tree>& trees = GetTrees();
        std::vector<Wall>& walls = GetWalls();
        std::vector<Window>& windows = GetWindows();

        for (AnimatedGameObject& animatedGameObject : animatedGameObjects) {
            animatedGameObject.Update(deltaTime);
        }

        for (BulletCasing& bulletCasing : bulletCasings) {
            bulletCasing.Update(deltaTime);
        }

        for (Door& door : doors) {
            door.Update(deltaTime);
        }

        for (GameObject& gameObject : gameObjects) {
            gameObject.Update(deltaTime);
        }

        for (Kangaroo& kangaroo : GetKangaroos()) {
            kangaroo.Update(deltaTime);
        }

        for (Mermaid& mermaid : GetMermaids()) {
            mermaid.Update(deltaTime);
        }

        for (PickUp& pickUp : pickUps) {
            pickUp.Update(deltaTime);
        }

        for (Piano& piano : pianos) {
            piano.Update(deltaTime);
        }

        for (Tree& tree : trees) {
            tree.Update(deltaTime);
        }

        for (Shark& shark : GetSharks()) {
            shark.Update(deltaTime);
        }

        for (Window& window : windows) {
            window.Update(deltaTime);
        }

        for (Wall& wall : walls) {
            // Nothing as of yet. Probably ever.
        }

        // Update lights last. That way their dirty state reflects the state of all 
        // objects whom may have potentially moved within their radius
        for (Light& light : lights) {
            light.Update(deltaTime);
        }

        CalculateGPULights();

        // Volumetric blood
        std::vector<VolumetricBloodSplatter>& volumetricBloodSplatters = GetVolumetricBloodSplatters();
        for (int i = 0; i < volumetricBloodSplatters.size(); i++) {
            VolumetricBloodSplatter& volumetricBloodSplatter = volumetricBloodSplatters[i];

            if (volumetricBloodSplatter.GetLifeTime() < 0.9f) {
                volumetricBloodSplatter.Update(deltaTime);
            }
            else {
                volumetricBloodSplatters.erase(volumetricBloodSplatters.begin() + i);
                i--;
            }
        }
    }

    void LazyDebugSpawns() {
        // AKs
        if (Input::KeyPressed(HELL_KEY_BACKSPACE)) {
            PickUpCreateInfo createInfo;
            createInfo.position = Game::GetLocalPlayerByIndex(0)->GetCameraPosition();
            createInfo.position += Game::GetLocalPlayerByIndex(0)->GetCameraForward();
            createInfo.rotation.x = Util::RandomFloat(-HELL_PI, HELL_PI);
            createInfo.rotation.y = Util::RandomFloat(-HELL_PI, HELL_PI);
            createInfo.rotation.z = Util::RandomFloat(-HELL_PI, HELL_PI);
            createInfo.pickUpType = Util::PickUpTypeToString(PickUpType::AKS74U);
            AddPickUp(createInfo);
        }

        // Remingtons
        if (Input::KeyPressed(HELL_KEY_INSERT)) {
            PickUpCreateInfo createInfo;
            createInfo.position = Game::GetLocalPlayerByIndex(0)->GetCameraPosition();
            createInfo.position += Game::GetLocalPlayerByIndex(0)->GetCameraForward();
            createInfo.rotation.x = Util::RandomFloat(-HELL_PI, HELL_PI);
            createInfo.rotation.y = Util::RandomFloat(-HELL_PI, HELL_PI);
            createInfo.rotation.z = Util::RandomFloat(-HELL_PI, HELL_PI);
            createInfo.pickUpType = Util::PickUpTypeToString(PickUpType::REMINGTON_870);
            AddPickUp(createInfo);
        }
    }

    void UpdateDoorAndWindowCubeTransforms() {
        std::vector<Transform>& transforms = GetDoorAndWindowCubeTransforms();
        std::vector<Door>& doors = GetDoors();
        std::vector<Window>& windows = GetWindows();

        transforms.clear();
        transforms.reserve(doors.size() + windows.size());

        for (Door& door : doors) {
            Transform& transform = transforms.emplace_back();
            transform.position = door.GetPosition();
            transform.position.y += DOOR_HEIGHT / 2;
            transform.rotation.y = door.GetRotation().y;
            transform.scale.x = 0.2f;
            transform.scale.y = DOOR_HEIGHT * 1.0f;
            transform.scale.z = 1.02f;
        }

        for (Window& window : windows) {
            float windowMidPointFromGround = 1.4f;

            Transform& transform = transforms.emplace_back();
            transform.position = window.GetPosition();
            transform.position.y += windowMidPointFromGround;
            transform.rotation.y = window.GetRotation().y;
            transform.scale.x = 0.2f;
            transform.scale.y = 1.2f;
            transform.scale.z = 0.846f;
        }
    }

    void CalculateGPULights() {
        for (int i = 0; i < GetLights().size(); i++) {
            RenderDataManager::SubmitGPULightHighRes(i);
        }
    }
}