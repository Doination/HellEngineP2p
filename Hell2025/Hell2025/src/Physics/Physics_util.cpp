#include "Physics.h"

namespace Physics {

    glm::vec3 PxVec3toGlmVec3(PxVec3 vec) {
        return { vec.x, vec.y, vec.z };
    }
    
    glm::vec3 PxVec3toGlmVec3(PxExtendedVec3 vec) {
        return { vec.x, vec.y, vec.z };
    }
    
    PxVec3 GlmVec3toPxVec3(glm::vec3 vec) {
        return { vec.x, vec.y, vec.z };
    }
    
    PxQuat GlmQuatToPxQuat(glm::quat quat) {
        return { quat.x, quat.y, quat.z, quat.w };
    }
    
    glm::quat PxQuatToGlmQuat(PxQuat quat) {
        return { quat.x, quat.y, quat.z, quat.w };
    }    

    glm::mat4 PxMat44ToGlmMat4(physx::PxMat44 pxMatrix) {
        glm::mat4 matrix;
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                matrix[x][y] = pxMatrix[x][y];
        return matrix;
    }

    physx::PxMat44 GlmMat4ToPxMat44(glm::mat4 glmMatrix) {
        physx::PxMat44 matrix;
        std::copy(glm::value_ptr(glmMatrix),
                  glm::value_ptr(glmMatrix) + 16,
                  reinterpret_cast<float*>(&matrix));
        return matrix;
    }

    PhysXRayResult CastPhysXRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float rayLength, PxU32 collisionFlags, bool cullBackFacing) {
        PxScene* scene = Physics::GetPxScene();
        PxVec3 origin = PxVec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
        PxVec3 unitDir = PxVec3(rayDirection.x, rayDirection.y, rayDirection.z);
        PxReal maxDistance = rayLength;
        PxRaycastBuffer hit;
        PxHitFlags outputFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eMESH_BOTH_SIDES;
        if (cullBackFacing) {
            outputFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;
        }

        // Only ray cast against objects with the GROUP_RAYCAST flag
        PxQueryFilterData filterData = PxQueryFilterData();
        filterData.data.word0 = collisionFlags;

        // Defaults
        PhysXRayResult result;
        result.hitObjectName = "NO_USERDATA";
        result.hitPosition = glm::vec3(0, 0, 0);
        result.surfaceNormal = glm::vec3(0, 0, 0);
        result.rayDirection = rayDirection;
        result.userData = PhysicsUserData();
        result.hitFound = scene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);

        // On hit
        if (result.hitFound) {
            result.hitPosition = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
            result.surfaceNormal = glm::vec3(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);
            result.hitFound = true;
            PhysicsUserData* userData = (PhysicsUserData*)hit.block.actor->userData;
            if (userData) {
                result.userData = *userData;
                result.hitObjectName = "HAS_USERDATA";
            }
        }
        return result;
    }

    PhysXOverlapReport OverlapTest(const PxGeometry& overlapShape, const PxTransform& shapePose, PxU32 collisionGroup) {
        PxScene* pxScene = Physics::GetPxScene();

        PxQueryFilterData overlapFilterData = PxQueryFilterData();
        overlapFilterData.data.word1 = collisionGroup;

        const PxU32 bufferSize = 256;
        PxOverlapHit hitBuffer[bufferSize];
        PxOverlapBuffer buf(hitBuffer, bufferSize);

        std::vector<PxActor*> hitActors;

        if (pxScene->overlap(overlapShape, shapePose, buf, overlapFilterData)) {
            for (int i = 0; i < buf.getNbTouches(); i++) {
                PxActor* hit = buf.getTouch(i).actor;
                // Check for duplicates
                bool found = false;
                for (const PxActor* foundHit : hitActors) {
                    if (foundHit == hit) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    hitActors.push_back(hit);
                }
            }
        }

        // Fill out the shit you need
        PhysXOverlapReport overlapReport;
        for (PxActor* hitActor : hitActors) {
            PhysicsUserData* userData = (PhysicsUserData*)hitActor->userData;
            if (userData) {
                if (userData->physicsType == PhysicsType::RIGID_DYNAMIC) {
                    PxRigidDynamic* rigid = (PxRigidDynamic*)hitActor;
                    PhysXOverlapResult& overlapResult = overlapReport.hits.emplace_back();
                    overlapResult.userData = *userData;
                    overlapResult.objectPosition.x = rigid->getGlobalPose().p.x;
                    overlapResult.objectPosition.y = rigid->getGlobalPose().p.y;
                    overlapResult.objectPosition.z = rigid->getGlobalPose().p.z;
                }
                if (userData->physicsType == PhysicsType::RIGID_STATIC ||
                    userData->physicsType == PhysicsType::HEIGHT_FIELD) {
                    PxRigidStatic* rigid = (PxRigidStatic*)hitActor;
                    PhysXOverlapResult& overlapResult = overlapReport.hits.emplace_back();
                    overlapResult.userData = *userData;
                    overlapResult.objectPosition.x = rigid->getGlobalPose().p.x;
                    overlapResult.objectPosition.y = rigid->getGlobalPose().p.y;
                    overlapResult.objectPosition.z = rigid->getGlobalPose().p.z;
                }
            }
        }
        return overlapReport;
    }
}

