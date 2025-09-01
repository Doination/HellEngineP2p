#pragma once
#include "Audio/Audio.h"
#include "HellDefines.h"
#include "HellEnums.h"

struct OpenStateHandler {
    OpenState openState;
    float currentOpenValue = 1;
    float minOpenValue = 0;
    float maxOpenValue = HELL_PI * 0.5f;
    float openSpeed = 1.0f;
    float closeSpeed = 1.0f;
    bool movedThisFrame = false;
    std::string openingAudio = "";
    std::string closingAudio = "";
    std::string openedAudio = "";
    std::string closedAudio = "";

    void Interact() {
        if (openState == OpenState::OPEN) {
            Audio::PlayAudio(openingAudio, 1.0f);
            openState = OpenState::CLOSING;
        }
        if (openState == OpenState::CLOSED) {
            Audio::PlayAudio(closingAudio, 1.0f);
            openState = OpenState::OPENING;
        }
    }

    void Update(float deltaTime) {
        movedThisFrame = false;

        if (openState == OpenState::CLOSING) {
            movedThisFrame = true;
            currentOpenValue -= closeSpeed * deltaTime;
            if (currentOpenValue < minOpenValue) {
                openState = OpenState::CLOSED;
                Audio::PlayAudio(closedAudio, 1.0f);
                currentOpenValue = minOpenValue;
            }
        }
        if (openState == OpenState::OPENING) {
            movedThisFrame = true;
            currentOpenValue += openSpeed * deltaTime;
            if (currentOpenValue > maxOpenValue) {
                openState = OpenState::OPEN;
                Audio::PlayAudio(openedAudio, 1.0f);
                currentOpenValue = maxOpenValue;
            }
        }
        if (openState == OpenState::OPEN) {
            currentOpenValue = maxOpenValue;
        }
        if (openState == OpenState::CLOSED) {
            currentOpenValue = minOpenValue;
        }
    }
};