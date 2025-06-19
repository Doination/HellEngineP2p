#pragma once
#pragma warning(push, 0)
#include <physx/characterkinematic/PxController.h>
#pragma warning(pop)
#include "HellTypes.h"
#include "Math/AABB.h"

using namespace physx;

struct CharacterController {
    void SetPxController(PxController* pxCharacterController);
    void MarkForRemoval();
    void SetGroundedState(bool state);
    void SetPosition(glm::vec3 position);
    void Move(const glm::vec3& displacement);
    const AABB GetAABB() const;
    glm::vec3 GetFootPosition();

    bool IsGrounded()                   { return m_grounded; }
    bool IsMarkedForRemoval()           { return m_markedForRemoval; }
    PxController* GetPxController()     { return m_pxController; }

private:
    PxController* m_pxController = nullptr;
    bool m_markedForRemoval = false;
    bool m_grounded = false;
};