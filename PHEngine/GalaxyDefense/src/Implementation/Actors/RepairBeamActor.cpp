#include "RepairBeamActor.h"

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RepairBeamComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
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
#include "Implementation/MissileExplosionVisitors/RepairBeamExplosionVisitor.h"

#include <glm/geometric.hpp>

#include <cmath>

using namespace EnginePhysics;
using namespace Event;

namespace Game {
namespace {
// Nanobots crawl from start to end at this fraction of the beam per second (spec: (i/8 + t * 0.42) % 1).
constexpr float c_nanobotTravelSpeed = 0.42f;
constexpr float c_tau = 6.28318530718f;
constexpr float c_pi = 3.14159265359f;
} // namespace

RepairBeamActor::RepairBeamActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : MissileActor(gameObjectName, rootComponent, combatActorsPoolHandler)
{
    mMissileType = eMissileType::REPAIR_BEAM;
}

void RepairBeamActor::Tick(const float deltaTimeSec, const float playSpeed)
{
    MissileActor::Tick(deltaTimeSec, playSpeed); // ticks the beam/nanobot/billboard components

    UpdateTargeting(); // raycasts onto the nearest barrier, registers the collision, or disables the beam
    if (!IsEnabled()) {
        return; // no target / spawner gone — the ray returned to the pool this frame
    }

    mElapsedSec += deltaTimeSec * playSpeed;
    UpdateNanobots();
    UpdatePulse();
}

bool RepairBeamActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mBeginWorldSpacePosition)
        || EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mEndWorldSpacePosition);
}

void RepairBeamActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType ownerType,
    const std::shared_ptr<SpaceStationActor>& spawnerActor)
{
    mDamageDealerType = ownerType;
    mSpawnerWp = spawnerActor;
    mElapsedSec = 0.0f;
    mIsCollided = false;
    mCollidedBarrierWp.reset();

    const auto spawnerPosition = spawnerActor->GetRootComponent()->GetTranslation();
    mBeamLineBegin = spawnerPosition;
    mBeamLineEnd = mBeamLineBegin;

    SetIsEnabled(true);
    mActivityState = eMissileActivityState::ACTIVE;
}

void RepairBeamActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;
    mBeamLineBegin = glm::vec3(0.0);
    mBeamLineEnd = glm::vec3(0.0);
    SetIsEnabled(false);
}

void RepairBeamActor::UpdateTargeting()
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
    if (collisionTest.HasHit()) {

        const auto& collidedPhysDescriptors = collisionTest.GetCollisionHitPhysicsDescriptors();
        std::vector<std::pair<int32_t /*owner actor id*/, int32_t /*physics descriptor id*/>> descriptorActorIds;
        std::transform(
            collidedPhysDescriptors.begin(),
            collidedPhysDescriptors.end(),
            std::back_inserter(descriptorActorIds),
            [](const auto& collidedDescriptor) {
                return std::make_pair(collidedDescriptor->GetOwnerActorEngineObjectId(), collidedDescriptor->GetId());
            });

        if (descriptorActorIds.size()) {
            const auto foundNearestIt = std::min_element(
                descriptorActorIds.begin(), descriptorActorIds.end(), [this](const auto& first, const auto& second) {
                    const auto [leftActorId, physDescriptorLeft] = first;
                    const auto [rightActorId, physDescriptorRight] = second;
                    const auto& leftBarrier = mCombatActorsPoolHandler->GetBarrierOwnerActorById(leftActorId);
                    const auto& rightBarrier = mCombatActorsPoolHandler->GetBarrierOwnerActorById(rightActorId);
                    if (leftBarrier && rightBarrier) {
                        const auto sqrDistanceToLeft
                            = glm::distance2(leftBarrier->GetRootComponent()->GetTranslation(), mBeamLineBegin);
                        const auto sqrDistanceToRight
                            = glm::distance2(rightBarrier->GetRootComponent()->GetTranslation(), mBeamLineBegin);
                        return sqrDistanceToLeft < sqrDistanceToRight;
                    }
                    return false;
                });
            if (foundNearestIt != descriptorActorIds.end()) {
                const auto& barrier = mCombatActorsPoolHandler->GetBarrierOwnerActorById(foundNearestIt->first);
                if (barrier && eBarrierActivityState::ACTIVE == barrier->GetState()) {
                    hitBarrier = barrier;
                    const int32_t pillarIndex = barrier->FindPillarIndexByPhysDescriptorId(foundNearestIt->second);
                    if (pillarIndex >= 0) {
                        mBeamLineEnd = barrier->GetBarrierPillarPosition(pillarIndex);
                    }
                }
            }
        }
    }

    if (hitBarrier) {
        if (!mIsCollided) {
            mIsCollided = true;
            mCollidedBarrierWp = hitBarrier;
            SendShootRayCollisionEvent(hitBarrier, eCollisionActionType::COLLISION_STARTED);
        }
        SetEndpoints(begin, mBeamLineEnd);
    } else {
        if (const auto& barrier = mCollidedBarrierWp.lock()) {
            SendShootRayCollisionEvent(barrier, eCollisionActionType::COLLISION_FINISHED);
        }
        TriggerDisabled(); // no barrier in the beam's path — back to the pool
    }
}

void RepairBeamActor::SendShootRayCollisionEvent(
    const std::shared_ptr<Actor>& collidedActor, const eCollisionActionType collisionActionType)
{
    ShootRayCollisionEvent::GetInstance()->SendEvent(
        eExecutionOrder::POST_EXECUTION,
        std::static_pointer_cast<MissileActor>(shared_from_this()),
        collidedActor,
        eRayType::REPAIR_BEAM,
        collisionActionType);
}

void RepairBeamActor::SetBeamComponent(const std::shared_ptr<EngineCore::RepairBeamComponent>& beamComponent)
{
    mBeamComponent = beamComponent;
}

void RepairBeamActor::SetNanobots(const std::vector<std::shared_ptr<EngineCore::InstancedStaticMeshComponent>>& nanobots)
{
    mNanobots = nanobots;
}

void RepairBeamActor::SetHealingCross(const std::shared_ptr<EngineCore::BillboardComponent>& healingCross)
{
    mHealingCross = healingCross;
}

void RepairBeamActor::SetBurstRings(const std::shared_ptr<EngineCore::BillboardComponent>& burstRings)
{
    mBurstRings = burstRings;
}

void RepairBeamActor::SetPulseBall(const std::shared_ptr<EngineCore::StaticMeshComponent>& pulseBall)
{
    mPulseBall = pulseBall;
}

std::shared_ptr<MissileExplosionVisitorBase> RepairBeamActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<RepairBeamExplosionVisitor>(std::static_pointer_cast<MissileActor>(shared_from_this()));
}

std::weak_ptr<SpaceStationActor> RepairBeamActor::GetActorWhoSpawnedMeWp() const
{
    return mSpawnerWp;
}

void RepairBeamActor::SetEndpoints(const glm::vec3& beginWorldSpacePosition, const glm::vec3& endWorldSpacePosition)
{
    mBeginWorldSpacePosition = beginWorldSpacePosition;
    mEndWorldSpacePosition = endWorldSpacePosition;
    if (mBeamComponent) {
        mBeamComponent->SetStartWorldPosition(beginWorldSpacePosition);
        mBeamComponent->SetEndWorldPosition(endWorldSpacePosition);
    }
    // The target-end billboards follow the (possibly moving) target.
    if (mHealingCross) {
        mHealingCross->SetTranslation(endWorldSpacePosition);
    }
    if (mBurstRings) {
        mBurstRings->SetTranslation(endWorldSpacePosition);
    }
}

glm::vec3 RepairBeamActor::GetWavePoint(const float t) const
{
    const glm::vec3 delta = mEndWorldSpacePosition - mBeginWorldSpacePosition;
    const float length = glm::length(delta);
    if (length < 0.0001f || !mBeamComponent) {
        return mBeginWorldSpacePosition;
    }
    const glm::vec3 direction = delta / length;

    // Same perpendicular basis the beam's world matrix uses, so riders sit exactly on the rendered wave plane.
    glm::vec3 tangent, bitangent;
    EngineMath::CreateOrthogonalBasisFromDirection(direction, tangent, bitangent);

    const float amplitude = mBeamComponent->GetWaveAmplitude();
    const float loopPeriod = mBeamComponent->GetWaveLoopPeriodSeconds();
    const float loopPhase
        = (loopPeriod > 0.0001f) ? (mElapsedSec / loopPeriod) * c_tau * RepairBeamComponent::c_waveTemporalCyclesPerLoop : 0.0f;

    const float window = std::sin(t * c_pi); // anchored to 0 at the ends, matching the beam's wave window
    const float phase = t * RepairBeamComponent::c_waveSpatialCycles * c_tau - loopPhase;
    const float displacement = std::sin(phase) * amplitude * window;

    return mBeginWorldSpacePosition + direction * (length * t) + tangent * displacement;
}

void RepairBeamActor::UpdateNanobots()
{
    if (mNanobots.empty()) {
        return;
    }

    const int32_t count = static_cast<int32_t>(mNanobots.size());
    for (int32_t i = 0; i < count; ++i) {
        const float t = std::fmod(static_cast<float>(i) / count + mElapsedSec * c_nanobotTravelSpeed, 1.0f);
        mNanobots[i]->SetTranslation(GetWavePoint(t));
    }
}

void RepairBeamActor::UpdatePulse()
{
    if (!mPulseBall) {
        return;
    }

    // One loop: the ball flies start -> end over the flight window, then hides while the burst rings fire and rest.
    const float phase = std::fmod(mElapsedSec, c_pulseCyclePeriodSec);
    const bool flying = phase < c_pulseFlightDurationSec;
    if (flying) {
        const float t = phase / c_pulseFlightDurationSec; // 0 -> 1 along the beam
        mPulseBall->SetTranslation(GetWavePoint(t));
        if (!mPulseBallVisible) {
            mPulseBall->SetIsVisible(true);
            mPulseBallVisible = true;
        }
    } else if (mPulseBallVisible) {
        mPulseBall->SetIsVisible(false);
        mPulseBallVisible = false;
    }
}
} // namespace Game
