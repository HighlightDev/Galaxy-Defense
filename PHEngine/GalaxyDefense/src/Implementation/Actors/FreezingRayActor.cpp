#include "FreezingRayActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/ElectricBeamComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/SphereCollisionTestWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Events/ShootRayCollisionEvent.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/MissileExplosionVisitors/FreezingRayExplosionVisitor.h"

#include <utility>

using namespace EnginePhysics;
using namespace Event;

namespace Game {
FreezingRayActor::FreezingRayActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : MissileActor(gameObjectName, rootComponent, combatActorsPoolHandler)
    , mLineComponent()
    , mActorWhoSpawnedMeWp()
    , mFreezingLineBegin()
    , mFreezingLineEnd()
    , mOpacity(std::make_shared<EngineObjectProperty<float>>(1.0f, "p_opacity"))
    , mSwitchTargetMinTimer(std::make_shared<GameThreadTimer>())
{
    mMissileType = eMissileType::FREEZING_RAY;
    AddEngineProperty(mOpacity);
    Initialize();
}

void FreezingRayActor::Initialize()
{
    constexpr size_t s_switchTargetMinTimeout = 1500;

    mSwitchTargetMinTimer->Initialize();
    mSwitchTargetMinTimer->SetIsPausable(true);
    mSwitchTargetMinTimer->SetIsRepeat(false);
    mSwitchTargetMinTimer->SetIntervalMs(s_switchTargetMinTimeout);
}

void FreezingRayActor::OnSceneOwnerInitialized()
{
    Actor::OnSceneOwnerInitialized();
    mSwitchTargetMinTimer->SetCallback(
        [weak_me = std::weak_ptr<FreezingRayActor>(std::static_pointer_cast<FreezingRayActor>(shared_from_this()))]() {
            if (auto shared_me = weak_me.lock()) {
                shared_me->OnCanSwitchTargetTimeout();
            }
        });
}

void FreezingRayActor::OnCanSwitchTargetTimeout()
{
    LogInfo("FreezingRayActor::OnCanSwitchTargetTimeout");
}

bool FreezingRayActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mFreezingLineBegin)
        || EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mFreezingLineBegin);
}

void FreezingRayActor::SendShootRayCollisionEvent(
    const std::shared_ptr<Actor>& collidedActor, const eCollisionActionType collisionActionType)
{
    Event::ShootRayCollisionEvent::GetInstance()->SendEvent(
        eExecutionOrder::POST_EXECUTION,
        std::static_pointer_cast<MissileActor>(shared_from_this()),
        collidedActor,
        eRayType::FREEZING_RAY,
        collisionActionType);
}

std::vector<std::shared_ptr<PhysicsComponent>> FreezingRayActor::CreateExcludedCollisionComponentsVector(
    const std::shared_ptr<PhysicsComponent>& collisionComponentOfWhoSpawnedMe) const
{
    std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionPhysComponents;
    const auto& spaceStations = mCombatActorsPoolHandler->GetSpaceStationActors();
    excludeCollisionPhysComponents.reserve(spaceStations.size() + mCombatActorsPoolHandler->GetMissileActors().size() + 1);

    if (collisionComponentOfWhoSpawnedMe) {
        excludeCollisionPhysComponents.emplace_back(collisionComponentOfWhoSpawnedMe);
    }
    const auto& spaceStationsPhysComponents = mCombatActorsPoolHandler->GetSpaceStationsPhysicsComponents();
    const auto& bombMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BOMB);
    const auto& freezeMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::FREEZING_BOMB);
    const auto& blackHoleMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BLACK_HOLE);
    excludeCollisionPhysComponents.insert(
        excludeCollisionPhysComponents.end(), spaceStationsPhysComponents.begin(), spaceStationsPhysComponents.end());
    excludeCollisionPhysComponents.insert(
        excludeCollisionPhysComponents.end(), bombMissilePhysComponents.begin(), bombMissilePhysComponents.end());
    excludeCollisionPhysComponents.insert(
        excludeCollisionPhysComponents.end(), freezeMissilePhysComponents.begin(), freezeMissilePhysComponents.end());
    excludeCollisionPhysComponents.insert(
        excludeCollisionPhysComponents.end(), blackHoleMissilePhysComponents.begin(), blackHoleMissilePhysComponents.end());

    return excludeCollisionPhysComponents;
}

void FreezingRayActor::Tick(const float deltaTimeSec)
{
    MissileActor::Tick(deltaTimeSec);
    ext_assert(mLineComponent, "FreezingRayActor line component is null");

    if (const auto& sceneSp = mSceneOwner.lock()) {
        if (const auto& actorWhoSpawnedMeSp = mActorWhoSpawnedMeWp.lock()) {
            mFreezingLineBegin = actorWhoSpawnedMeSp->GetRootComponent()->GetTranslation();
            const auto& ownerPhysComp = actorWhoSpawnedMeSp->GetPhysicsComponent();
            const auto freezingRayHitRadius = actorWhoSpawnedMeSp->GetSpaceStationLevel()->GetShootRadius();
            auto sphereCollisionTest = SphereCollisionTestWithFilterAdapter(
                freezingRayHitRadius, CreateExcludedCollisionComponentsVector(ownerPhysComp));
            sphereCollisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), mFreezingLineBegin);

            if (sphereCollisionTest.HasHit()) {
                const auto& collidedPhysDescriptors = sphereCollisionTest.GetCollisionHitPhysicsDescriptors();
                std::vector<int32_t> descriptorActorIds;
                std::transform(
                    collidedPhysDescriptors.begin(),
                    collidedPhysDescriptors.end(),
                    std::back_inserter(descriptorActorIds),
                    [](const auto& collidedDescriptor) { return collidedDescriptor->GetOwnerActorEngineObjectId(); });

                if (descriptorActorIds.size()) {
                    const auto foundNearestIt = std::min_element(
                        descriptorActorIds.begin(),
                        descriptorActorIds.end(),
                        [this](const auto& leftActorId, const auto& rightActorId) {
                            const auto& leftShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(leftActorId);
                            const auto& rightShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(rightActorId);
                            if (leftShipActor && rightShipActor) {
                                const auto sqrDistanceToLeft
                                    = glm::distance2(leftShipActor->GetRootComponent()->GetTranslation(), mFreezingLineBegin);
                                const auto sqrDistanceToRight
                                    = glm::distance2(rightShipActor->GetRootComponent()->GetTranslation(), mFreezingLineBegin);
                                return sqrDistanceToLeft < sqrDistanceToRight;
                            }
                            return false;
                        });
                    if (foundNearestIt != descriptorActorIds.end()) {
                        const auto& collidedActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(*foundNearestIt);
                        if (collidedActor) {
                            LogInfo("FreezingRayActor::Tick: collided with actor ", collidedActor->GetName());
                            const bool mCollideWithOldActor
                                = mLastCollidedActorId == collidedActor->GetObjectId() || mSwitchTargetMinTimer->IsRunning();
                            if (mCollideWithOldActor) {
                                const auto& previousCollidedActor
                                    = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(mLastCollidedActorId);
                                if (previousCollidedActor && previousCollidedActor->IsEnabled()
                                    && previousCollidedActor->IsVisible()) {
                                    SendShootRayCollisionEvent(
                                        previousCollidedActor->shared_from_this(), eCollisionActionType::COLLISION_STARTED);
                                    mFreezingLineEnd = previousCollidedActor->GetRootComponent()->GetTranslation();
                                } else {
                                    if (previousCollidedActor) {
                                        SendShootRayCollisionEvent(
                                            previousCollidedActor->shared_from_this(), eCollisionActionType::COLLISION_FINISHED);
                                    }
                                    mLastCollidedActorId = -1;
                                    mSwitchTargetMinTimer->StopTimer();
                                }
                            }
                        } else {
                            if (mLastCollidedActorId != -1) {
                                const auto& previousCollidedActor
                                    = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(mLastCollidedActorId);
                                if (previousCollidedActor) {
                                    SendShootRayCollisionEvent(
                                        previousCollidedActor->shared_from_this(), eCollisionActionType::COLLISION_FINISHED);
                                }
                            }

                            mLastCollidedActorId = collidedActor->GetObjectId();
                            SendShootRayCollisionEvent(
                                collidedActor->shared_from_this(), eCollisionActionType::COLLISION_STARTED);
                            mSwitchTargetMinTimer->StartTimer();
                            mFreezingLineEnd = collidedActor->GetRootComponent()->GetTranslation();
                        }
                    }
                } else {
                    TriggerDisabled();
                    if (mLastCollidedActorId != -1) {
                        const auto& previousCollidedActor
                            = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(mLastCollidedActorId);
                        if (previousCollidedActor) {
                            SendShootRayCollisionEvent(
                                previousCollidedActor->shared_from_this(), eCollisionActionType::COLLISION_FINISHED);
                        }
                        mLastCollidedActorId = -1;
                    }
                }
            }
        }
    }

    mLineComponent->SetStartWorldPosition(mFreezingLineBegin);
    mLineComponent->SetEndWorldPosition(mFreezingLineEnd);
}

void FreezingRayActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType ownerType,
    const std::shared_ptr<SpaceStationActor>& spawnerActor)
{
    LogInfo("FreezingRayActor::TriggerSpawn");
    mDamageDealerType = ownerType;
    mActorWhoSpawnedMeWp = spawnerActor;
    DropState();

    const auto spawnerPosition = spawnerActor->GetRootComponent()->GetTranslation();
    mFreezingLineBegin = spawnerPosition;
    mFreezingLineEnd = spawnerPosition;

    SetIsEnabled(true);
    mActivityState = eMissileActivityState::ACTIVE;
}

void FreezingRayActor::TriggerExplosion()
{
    mActivityState = eMissileActivityState::EXPLOSION;
}

void FreezingRayActor::TriggerExplosionFinished()
{
    mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
}

void FreezingRayActor::TriggerDisabled()
{
    LogInfo("FreezingRayActor::TriggerDisabled");
    mActivityState = eMissileActivityState::IDLE;
    DropState();
    SetIsEnabled(false);
}

std::shared_ptr<MissileExplosionVisitorBase> FreezingRayActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<FreezingRayExplosionVisitor>(std::static_pointer_cast<FreezingRayActor>(shared_from_this()));
}

void FreezingRayActor::SetLineComponent(const std::shared_ptr<::EngineCore::ElectricBeamComponent>& lineComponent)
{
    mLineComponent = lineComponent;
}

void FreezingRayActor::DropState()
{
    mFreezingLineBegin = glm::vec3(0.0f);
    mFreezingLineEnd = glm::vec3(0.0f);
    mLastCollidedActorId = -1;
    mSwitchTargetMinTimer->StopTimer();
}

std::weak_ptr<SpaceStationActor> FreezingRayActor::GetActorWhoSpawnedMeWp() const
{
    return mActorWhoSpawnedMeWp;
}
} // namespace Game
