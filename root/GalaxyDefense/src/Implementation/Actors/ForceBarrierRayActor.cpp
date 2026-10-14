#include "ForceBarrierRayActor.h"

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/ForceBarrierBeamComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/SphereCollisionTestWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Events/ShootRayCollisionEvent.h"
#include "Implementation/GameObjectsType.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/MissileExplosionVisitors/ForceBarrierRayExplosionVisitor.h"

#include <glm/geometric.hpp>

#include <algorithm>
#include <utility>
#include <vector>

using namespace EnginePhysics;
using namespace Event;

namespace Game {
namespace {
// The shield stands off the barrier on the camera-facing side by this many world units, so it is not buried inside the
// barrier and reads as a flat plate facing the viewer.
constexpr float c_shieldCameraStandoff = 6.0f;
} // namespace

ForceBarrierRayActor::ForceBarrierRayActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : MissileActor(gameObjectName, rootComponent, combatActorsPoolHandler)
{
    mMissileType = eMissileType::FORCE_BARRIER;
}

void ForceBarrierRayActor::Tick(const float deltaTimeSec, const float playSpeed)
{
    MissileActor::Tick(deltaTimeSec, playSpeed); // ticks the beam/shield components (self-animating)
    UpdateTargeting();
}

bool ForceBarrierRayActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mBeginWorldSpacePosition)
        || EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mEndWorldSpacePosition);
}

void ForceBarrierRayActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType ownerType,
    const std::shared_ptr<SpaceStationActor>& spawnerActor)
{
    mDamageDealerType = ownerType;
    mSpawnerWp = spawnerActor;
    mIsCollided = false;
    mCollidedBarrierWp.reset();

    const glm::vec3 spawnerPosition
        = spawnerActor ? spawnerActor->GetRootComponent()->GetHierarchyAccumulatedTranslation() : position;
    SetEndpoints(spawnerPosition, spawnerPosition); // collapsed at the tower until a barrier is targeted

    SetIsEnabled(true);
    mActivityState = eMissileActivityState::ACTIVE;
}

void ForceBarrierRayActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;
    SetIsEnabled(false);
}

void ForceBarrierRayActor::SetBeamComponent(const std::shared_ptr<EngineCore::ForceBarrierBeamComponent>& beamComponent)
{
    mBeamComponent = beamComponent;
}

void ForceBarrierRayActor::SetShield(const std::shared_ptr<EngineCore::BillboardComponent>& shield)
{
    mShield = shield;
}

std::shared_ptr<MissileExplosionVisitorBase> ForceBarrierRayActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<ForceBarrierRayExplosionVisitor>(std::static_pointer_cast<MissileActor>(shared_from_this()));
}

std::weak_ptr<SpaceStationActor> ForceBarrierRayActor::GetActorWhoSpawnedMeWp() const
{
    return mSpawnerWp;
}

void ForceBarrierRayActor::UpdateTargeting()
{
    const auto spawner = mSpawnerWp.lock();
    const auto sceneSp = mSceneOwner.lock();
    if (!spawner || !sceneSp || !mCombatActorsPoolHandler || eSpaceStationActivityState::ACTIVE != spawner->GetState()) {
        if (const auto& barrier = mCollidedBarrierWp.lock()) {
            SendShootRayCollisionEvent(barrier, eCollisionActionType::COLLISION_FINISHED);
        }
        TriggerDisabled();
        return;
    }

    const glm::vec3 begin = spawner->GetRootComponent()->GetHierarchyAccumulatedTranslation();

    // The beam connects to a barrier, so exclude everything else and look for the nearest barrier pillar in range.
    auto excludeComponents = mCombatActorsPoolHandler->GetPhysicsComponentsByGameObjectTypes(
        {eGameObjectsType::SPACE_STATION,
         eGameObjectsType::TOWER_MISSILE,
         eGameObjectsType::SPACESHIP_MISSILE,
         eGameObjectsType::SPACESHIP,
         eGameObjectsType::NEUTRAL_SPACE_OBJECT,
         eGameObjectsType::LOOT});

    auto collisionTest
        = SphereCollisionTestWithFilterAdapter(spawner->GetSpaceStationLevel()->GetShootRadius(), excludeComponents);
    collisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), begin);

    std::shared_ptr<BarrierActor> hitBarrier;
    glm::vec3 endPoint = begin;
    if (collisionTest.HasHit()) {
        const auto& hitDescriptors = collisionTest.GetCollisionHitPhysicsDescriptors();
        std::shared_ptr<BarrierActor> nearestBarrier;
        int32_t nearestDescriptorId = -1;
        float nearestDistSq = -1.0f;
        for (const auto& descriptor : hitDescriptors) {
            const auto& barrier = mCombatActorsPoolHandler->GetBarrierOwnerActorById(descriptor->GetOwnerActorEngineObjectId());
            if (!barrier || eBarrierActivityState::ACTIVE != barrier->GetState()) {
                continue;
            }

            const glm::vec3 toBarrierVec = barrier->GetRootComponent()->GetTranslation() - begin;
            const float distSq = glm::dot(toBarrierVec, toBarrierVec);
            if (nearestDistSq < 0.0f || distSq < nearestDistSq) {
                nearestDistSq = distSq;
                nearestBarrier = barrier;
                nearestDescriptorId = descriptor->GetId();
            }
        }
        if (nearestBarrier) {
            hitBarrier = nearestBarrier;
            const int32_t pillarIndex = nearestBarrier->FindPillarIndexByPhysDescriptorId(nearestDescriptorId);
            endPoint = (pillarIndex >= 0) ? nearestBarrier->GetBarrierPillarPosition(pillarIndex)
                                          : nearestBarrier->GetRootComponent()->GetTranslation();
        }
    }

    if (hitBarrier) {
        if (!mIsCollided) {
            mIsCollided = true;
            mCollidedBarrierWp = hitBarrier;
            SendShootRayCollisionEvent(hitBarrier, eCollisionActionType::COLLISION_STARTED);
        }
        SetEndpoints(begin, endPoint);
    } else {
        if (const auto& barrier = mCollidedBarrierWp.lock()) {
            SendShootRayCollisionEvent(barrier, eCollisionActionType::COLLISION_FINISHED);
        }
        TriggerDisabled();
    }
}

void ForceBarrierRayActor::SendShootRayCollisionEvent(
    const std::shared_ptr<Actor>& collidedActor, const eCollisionActionType collisionActionType)
{
    ShootRayCollisionEvent::GetInstance()->SendEvent(
        eExecutionOrder::POST_EXECUTION,
        std::static_pointer_cast<MissileActor>(shared_from_this()),
        collidedActor,
        eRayType::FORCE_BARRIER,
        collisionActionType);
}

void ForceBarrierRayActor::SetEndpoints(const glm::vec3& beginWorldSpacePosition, const glm::vec3& endWorldSpacePosition)
{
    mBeginWorldSpacePosition = beginWorldSpacePosition;
    mEndWorldSpacePosition = endWorldSpacePosition;
    if (mBeamComponent) {
        mBeamComponent->SetStartWorldPosition(beginWorldSpacePosition);
        mBeamComponent->SetEndWorldPosition(endWorldSpacePosition);
    }
    if (mShield) {
        // Deploy the shield on the camera-facing side of the impact point (not embedded in the barrier). Recomputed
        // each frame from the live camera position, so it always stands between the barrier and the viewer.
        glm::vec3 shieldPosition = endWorldSpacePosition;
        if (const auto sceneSp = mSceneOwner.lock()) {
            if (const auto& camera = sceneSp->GetMainCamera()) {
                const glm::vec3 toCamera = camera->GetEyeVector() - endWorldSpacePosition;
                const float distance = glm::length(toCamera);
                if (distance > 0.0001f) {
                    shieldPosition += (toCamera / distance) * c_shieldCameraStandoff;
                }
            }
        }
        mShield->SetTranslation(shieldPosition);
    }
}
} // namespace Game
