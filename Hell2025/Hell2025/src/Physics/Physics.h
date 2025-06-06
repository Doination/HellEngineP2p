#pragma once
#pragma warning(push, 0)
#include <physx/PxPhysicsAPI.h>
#include <physx/geometry/PxGeometryHelpers.h>
#include <physx/PxQueryFiltering.h>
#pragma warning(pop)
#include "Physics/Types/D6Joint.h"
#include "Physics/Types/HeightField.h"
#include "Physics/Types/Ragdoll.h"
#include "Physics/Types/RigidDynamic.h"
#include "CollisionReports.h"
#include "HellTypes.h"
#include "Math/AABB.h"
#include <string>
#include <span>
#include <vector>


using namespace physx;

struct RaycastFilterCallback : PxQueryFilterCallback {
    PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
    PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) override;

    std::vector<PxRigidActor*> m_ignoredActors;
    void AddIgnoredActor(PxRigidDynamic* pxRigidDynamic);
    void AddIgnoredActors(std::vector<PxRigidDynamic*> pxRigidDynamics);
};

struct RaycastHeightFieldFilterCallback : PxQueryFilterCallback {
    PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
    PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) override;
};

namespace Physics {
    void Init();
    void BeginFrame();
    void StepPhysics(float deltaTime);
    void ForceZeroStepUpdate();
    void SubmitDebugLinesToRenderer(const DebugRenderMode& debugRenderMode);
    void AddCollisionReport(CollisionReport& collisionReport);
    void ClearCollisionReports();
    void ClearCharacterControllerCollsionReports();
    std::vector<CollisionReport>& GetCollisionReports();
    std::vector<CharacterCollisionReport>& GetCharacterCollisionReports();
    PxPhysics* GetPxPhysics();
    PxScene* GetPxScene();
    CCTHitCallback& GetCharacterControllerHitCallback();    
    PxControllerManager* GetCharacterControllerManager();

    // Ragdolls
    void LoadRagdollsFromDisk();
    uint64_t CreateRagdollByName(const std::string& name, float totalRagdollWeight);
    Ragdoll* GetRagdollById(uint64_t ragdollId);
    bool RagdollExists(uint64_t ragdollId);
    void RemoveRagdoll(uint64_t ragdollId);
    bool PxRigidDynamicBelongsToRagdoll(PxRigidDynamic* pxRigidDynamic);
    std::vector<PxRigidDynamic*> GetRagdollPxRigidDynamics(uint64_t ragdollId);
    std::vector<PxRigidActor*> GetRagdollPxRigidActors(uint64_t ragdollId);

    // Materials
    PxMaterial* GetDefaultMaterial();
    PxMaterial* GetGrassMaterial();

    // Create
    PxShape* CreateBoxShape(float width, float height, float depth, Transform shapeOffset = Transform(), PxMaterial* material = NULL);
    PxRigidDynamic* CreateRigidDynamic(Transform worldTransform, PhysicsFilterData filterData, PxShape* shape, Transform shapeOffset = Transform());
    PxRigidDynamic* CreateRigidDynamic(PxShape* shape, glm::mat4 worldMatrix, glm::mat4 shapeOffsetMatrix, PhysicsFilterData filterData);

    PxShape* CreateConvexShapeFromVertexList(std::span<Vertex>& vertices);

    // Height fields
    void UpdateHeightFields();
    void CreateHeightField(vecXZ& worldSpaceOffset, const float* heightValues);
    void MarkAllHeightFieldsForRemoval();
    void RemoveAnyHeightFieldMarkedForRemoval();
    const std::vector<HeightField>& GetHeightFields();
    void ActivateAllHeightFields();

    // Rigid Dynamics
    void UpdateActiveRigidDynamicAABBList();
    void MarkRigidDynamicForRemoval(uint64_t rigidDynamicId);
    void RemoveRigidDynamic(uint64_t rigidDynamicId);
    void AddFoceToRigidDynamic(uint64_t rigidDynamicId, glm::vec3 force);
    bool RigidDynamicExists(uint64_t rigidDynamicId);
    uint64_t CreateRigidDynamicFromConvexMeshVertices(Transform transform, const std::span<Vertex>& vertices, const std::span<uint32_t>& indices, float mass, PhysicsFilterData filterData, glm::vec3 initialForce = glm::vec3(0.0f), glm::vec3 initialTorque = glm::vec3(0.0f));
    uint64_t CreateRigidDynamicFromBoxExtents(Transform transform, glm::vec3 boxExtents, float mass, PhysicsFilterData filterData, glm::vec3 initialForce = glm::vec3(0.0f), glm::vec3 initialTorque = glm::vec3(0.0f));
    uint64_t CreateRigidDynamicFromPxShape(PxShape* pxShape, glm::mat4 initialPose, glm::mat4 shapeOffsetMatrix);
    glm::mat4 GetRigidDynamicWorldMatrix(uint64_t rigidDynamicId);
    void RemoveAnyRigidDynamicMarkedForRemoval();
    void ActivateRigidDynamicPhysics(uint64_t rigidDynamicId);
    void DeactivateRigidDynamicPhysics(uint64_t rigidDynamicId);
    void SetRigidDynamicUserData(uint64_t rigidDynamicId, PhysicsUserData physicsUserData);
    void UpdateAllRigidDynamics(float deltaTime);
    void SetRigidDynamicGlobalPose(uint64_t rigidDynamicId, glm::mat4 globalPoseMatrix);
    const std::vector<AABB>& GetActiveRididDynamicAABBs();
    RigidDynamic* GetRigidDynamicById(uint64_t rigidDynamicId);

    // Rigid statics
    void MarkRigidStaticForRemoval(uint64_t rigidStaticId);
    void RemoveRigidStatic(uint64_t rigidStaticId);
    void RemoveAnyRigidStaticMarkedForRemoval();
    void SetRigidStaticGlobalPose(uint64_t rigidStaticId, glm::mat4 globalPoseMatrix);
    glm::mat4 GetRigidStaticGlobalPose(uint64_t rigidStaticId);
    bool RigidStaticExists(uint64_t rigidStaticId);
    void SetRigidStaticUserData(uint64_t rigidStaticId, PhysicsUserData physicsUserData);
    uint64_t CreateRigidStaticFromCapsule(Transform transform, float radius, float halfHeight, PhysicsFilterData filterData, Transform localOffset);
    uint64_t CreateRigidStaticBoxFromExtents(Transform transform, glm::vec3 boxExtents, PhysicsFilterData filterData, Transform localOffset = Transform());
    uint64_t CreateRigidStaticConvexMeshFromModel(Transform transform, const std::string& modelName, PhysicsFilterData filterData);
    uint64_t CreateRigidStaticConvexMeshFromVertices(Transform transform, const std::span<Vertex>& vertices, PhysicsFilterData filterData);
    uint64_t CreateRigidStaticTriangleMeshFromVertexData(Transform transform, const std::span<Vertex>& vertices, const std::span<uint32_t>& indices, PhysicsFilterData filterData);
    uint64_t CreateRigidStaticTriangleMeshFromModel(Transform transform, const std::string& modelName, PhysicsFilterData filterData);
    
    // D6Joints
    uint64_t CreateD6Joint(uint64_t parentRigidDynamicId, uint64_t childRigidDynamicId, glm::mat4 parentFrame, glm::mat4 childFrame);
    D6Joint* GetD6JointById(uint64_t d6JointId);
    bool D6JointExists(uint64_t d6JointId);
    void RemoveD6Joint(uint64_t d6JointId);

    // Destroy
    void Destroy(PxRigidDynamic*& rigidDynamic);
    void Destroy(PxRigidStatic*& rigidStatic);
    void Destroy(PxShape*& shape);
    void Destroy(PxRigidBody*& rigidBody);
    void Destroy(PxTriangleMesh*& triangleMesh);

    // Debug
    void PrintSceneInfo();
    void PrintSceneD6JointInfo();
    void PrintSceneRigidInfo();
    void PrintSceneRagdollInfo();

    // Util
    std::vector<PxRigidActor*> GetIgnoreList(RaycastIgnoreFlags flags);
    std::string GetPxShapeTypeAsString(PxShape* pxShape);
    float ComputeShapeVolume(PxShape* pxShape);
    glm::vec3 PxVec3toGlmVec3(PxVec3 vec);
    glm::vec3 PxVec3toGlmVec3(PxExtendedVec3 vec);
    glm::quat PxQuatToGlmQuat(PxQuat quat);
    glm::mat4 PxMat44ToGlmMat4(physx::PxMat44 pxMatrix);
    PxVec3 GlmVec3toPxVec3(glm::vec3 vec);
    PxQuat GlmQuatToPxQuat(glm::quat quat);
    PxMat44 GlmMat4ToPxMat44(glm::mat4 glmMatrix);
    PhysXRayResult CastPhysXRayHeightMap(glm::vec3 rayOrigin, glm::vec3 rayDirection, float rayLength);
    PhysXRayResult CastPhysXRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float rayLength, bool cullBackFacing = false, RaycastIgnoreFlags ignoreFlags = RaycastIgnoreFlags(), std::vector<PxRigidActor*> ignoredActors = std::vector<PxRigidActor*>());
    PhysXOverlapReport OverlapTest(const PxGeometry& overlapShape, const PxTransform& shapePose, PxU32 collisionGroup);
}

