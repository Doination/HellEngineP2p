#pragma once
#pragma warning(push, 0)
#include <physx/PxShape.h>
#include <physx/PxRigidStatic.h>
#pragma warning(pop)

#include "Math/AABB.h"
#include <vector>

using namespace physx;

struct RigidStatic {
    void Update(float deltaTime);
    void SetPxRigidStatic(PxRigidStatic* rigidStatic);
    void AddPxShape(PxShape* shape);
    void MarkForRemoval();

    bool HasActivePhysics()                 { return m_activePhysics; }
    bool IsMarkedForRemoval()               { return m_markedForRemoval; }
    PxRigidStatic* GetPxRigidStatic()       { return m_pxRigidStatic; }
    std::vector<PxShape*>& GetPxShapes()    { return m_pxShapes; }
    const AABB& GetAABB()                   { return m_aabb; }
    const glm::vec3& GetCurrentPosition()   { return m_currentPosition; }
    const glm::vec3& GetPreviousPosition()  { return m_previousPosition; }

private:
    AABB m_aabb;
    std::vector<PxShape*> m_pxShapes;
    PxRigidStatic* m_pxRigidStatic = nullptr;
    glm::vec3 m_currentPosition;
    glm::vec3 m_previousPosition;
    float m_stationaryTime = 0;
    bool m_activePhysics = false;
    bool m_markedForRemoval = false;
};