#include "Kangaroo.h"
#include "Audio/Audio.h"
#include "Core/Game.h"

void Kangaroo::UpdateAudio() {
    AnimatedGameObject* animatedGameObject = GetAnimatedGameObject();
    if (!animatedGameObject) return;

    // Start ambient loop
    if (m_ambientLoopAudioHandle == 0) {
        m_ambientLoopAudioHandle = Audio::LoopAudioIfNotPlaying("Kangaroo_AmbientLoop.wav", 1.0f);
    }

    // Set ambient loop volume based on player proximity
    Player* player = Game::GetLocalPlayerByIndex(0);
    float distanceToPlayer = glm::distance(m_position, player->GetCameraPosition());
    float minDistance = 10.0f;
    float maxDistance = 20.0f;
    float volume = 0.0f;
    if (distanceToPlayer <= minDistance) {
        volume = 1.0f;
    }
    else if (distanceToPlayer >= maxDistance) {
        volume = 0.0f;
    }
    else {
        float t = (distanceToPlayer - minDistance) / (maxDistance - minDistance);
        volume = glm::mix(1.0f, 0.0f, t);
    }
    Audio::SetVolume(m_ambientLoopAudioHandle, volume);

    // Hop steps
    if (m_animationState == KanagarooAnimationState::HOP) {


        // Hack reset the flag
        if (animatedGameObject->GetAnimationFrameNumber() < 3) {
            m_awaitingHopStepAudio = true;
        }

        if (animatedGameObject->GetAnimationFrameNumber() >= 6 && m_awaitingHopStepAudio) {
            std::vector<const char*> filenames = {
                           "unloved_kingkroo_footstep_01.wav",
                           "unloved_kingkroo_footstep_02.wav",
                           "unloved_kingkroo_footstep_03.wav",
                           "unloved_kingkroo_footstep_04.wav",
                           "unloved_kingkroo_footstep_05.wav",
                           "unloved_kingkroo_footstep_06.wav",
                           "unloved_kingkroo_footstep_07.wav",
                           "unloved_kingkroo_footstep_08.wav",
                           "unloved_kingkroo_footstep_09.wav",
                           "unloved_kingkroo_footstep_10.wav",
                           "unloved_kingkroo_footstep_11.wav",
                           "unloved_kingkroo_footstep_12.wav",
                           "unloved_kingkroo_footstep_13.wav",
                           "unloved_kingkroo_footstep_14.wav",
                           "unloved_kingkroo_footstep_15.wav",
            };
            int random = rand() % filenames.size();
            Audio::PlayAudio(filenames[random], 1.0f);
            m_awaitingHopStepAudio = false;

            std::cout << "played hop step\n";
        }
    } 
    else {
        m_awaitingHopStepAudio = false;
    }
}

void Kangaroo::PlayFleshAudio() {
    std::vector<const char*> filenames = {
                           "FLY_Bullet_Impact_Flesh_00.wav",
                           "FLY_Bullet_Impact_Flesh_01.wav",
                           "FLY_Bullet_Impact_Flesh_02.wav",
                           "FLY_Bullet_Impact_Flesh_03.wav",
                           "FLY_Bullet_Impact_Flesh_04.wav",
                           "FLY_Bullet_Impact_Flesh_05.wav",
                           "FLY_Bullet_Impact_Flesh_06.wav",
                           "FLY_Bullet_Impact_Flesh_07.wav",
    };
    int random = rand() % filenames.size();
    Audio::PlayAudio(filenames[random], 1.0f);
}


void Kangaroo::PlayBiteSound() {
    std::vector<const char*> filenames = {
                           "unloved_kingkroo_growl_bite_01.wav",
                           "unloved_kingkroo_growl_bite_02.wav",
                           "unloved_kingkroo_growl_bite_03.wav",
                           "unloved_kingkroo_growl_bite_04.wav",
    };
    int random = rand() % filenames.size();
    Audio::PlayAudio(filenames[random], 1.0f);
}