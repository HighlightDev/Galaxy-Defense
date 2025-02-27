#include "FreezingRayActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
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
    , mElectroLineBegin()
    , mElectroLineEnd()
    , mOpacity(std::make_shared<EngineObjectProperty<float>>(1.0f, "p_opacity"))
{
    mMissileType = eMissileType::FREEZING_RAY;
    AddEngineProperty(mOpacity);
    Initialize();
}

void FreezingRayActor::Initialize()
{
}

bool FreezingRayActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mElectroLineBegin)
        || EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mElectroLineBegin);
}

void FreezingRayActor::SetFreezingRayHitRadius(const float radius)
{
    mFreezingRayHitRadius = radius;
}

void FreezingRayActor::Tick(const float deltaTime)
{
    MissileActor::Tick(deltaTime);
    assert(mLineComponent);

    if (const auto& sceneSp = mSceneOwner.lock()) {
        if (const auto& actorWhoSpawnedMeSp = mActorWhoSpawnedMeWp.lock()) {
            mElectroLineBegin = actorWhoSpawnedMeSp->GetRootComponent()->GetTranslation();
            std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionPhysComponents;
            const auto& spaceStations = mCombatActorsPoolHandler->GetSpaceStationActors();
            excludeCollisionPhysComponents.reserve(
                spaceStations.size() + mCombatActorsPoolHandler->GetMissileActors().size() + 1);
            if (const auto& ownerPhysComp = actorWhoSpawnedMeSp->GetPhysicsComponent()) {
                excludeCollisionPhysComponents.emplace_back(ownerPhysComp);
            }
            const auto& spaceStationsPhysComponents = mCombatActorsPoolHandler->GetSpaceStationsPhysicsComponents();
            const auto& bombMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BOMB);
            const auto& freezeMissilePhysComponents
                = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::FREEZING);
            const auto& blackHoleMissilePhysComponents
                = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BLACK_HOLE);
            excludeCollisionPhysComponents.insert(
                excludeCollisionPhysComponents.end(), spaceStationsPhysComponents.begin(), spaceStationsPhysComponents.end());
            excludeCollisionPhysComponents.insert(
                excludeCollisionPhysComponents.end(), bombMissilePhysComponents.begin(), bombMissilePhysComponents.end());
            excludeCollisionPhysComponents.insert(
                excludeCollisionPhysComponents.end(), freezeMissilePhysComponents.begin(), freezeMissilePhysComponents.end());
            excludeCollisionPhysComponents.insert(
                excludeCollisionPhysComponents.end(),
                blackHoleMissilePhysComponents.begin(),
                blackHoleMissilePhysComponents.end());

            auto sphereCollisionTest
                = SphereCollisionTestWithFilterAdapter(mFreezingRayHitRadius, excludeCollisionPhysComponents);
            sphereCollisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), mElectroLineBegin);

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
                            assert(leftShipActor && rightShipActor);
                            const auto sqrDistanceToLeft
                                = glm::distance2(leftShipActor->GetRootComponent()->GetTranslation(), mElectroLineBegin);
                            const auto sqrDistanceToRight
                                = glm::distance2(rightShipActor->GetRootComponent()->GetTranslation(), mElectroLineBegin);
                            return sqrDistanceToLeft < sqrDistanceToRight;
                        });
                    if (foundNearestIt != descriptorActorIds.end()) {
                        const auto& collidedActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(*foundNearestIt);
                        assert(collidedActor);
                        if (mLastCollidedActorId != collidedActor->GetObjectId()) {
                            if (mLastCollidedActorId != -1) {
                                const auto& previousCollidedActor
                                    = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(mLastCollidedActorId);
                                assert(previousCollidedActor);
                                Event::ShootRayCollisionEvent::GetInstance()->SendEvent(
                                    eExecutionOrder::POST_EXECUTION,
                                    std::static_pointer_cast<MissileActor>(shared_from_this()),
                                    previousCollidedActor->shared_from_this(),
                                    eRayType::FREEZING_RAY,
                                    eCollisionActionType::COLLISION_FINISHED);
                            }
                            mLastCollidedActorId = collidedActor->GetObjectId();
                            Event::ShootRayCollisionEvent::GetInstance()->SendEvent(
                                eExecutionOrder::POST_EXECUTION,
                                std::static_pointer_cast<MissileActor>(shared_from_this()),
                                collidedActor->shared_from_this(),
                                eRayType::FREEZING_RAY,
                                eCollisionActionType::COLLISION_STARTED);
                        }
                        mElectroLineEnd = collidedActor->GetRootComponent()->GetTranslation();
                    }
                } else {
                    TriggerDisabled();
                    if (mLastCollidedActorId != -1) {
                        const auto& previousCollidedActor
                            = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(mLastCollidedActorId);
                        assert(previousCollidedActor);
                        Event::ShootRayCollisionEvent::GetInstance()->SendEvent(
                            eExecutionOrder::POST_EXECUTION,
                            std::static_pointer_cast<MissileActor>(shared_from_this()),
                            previousCollidedActor->shared_from_this(),
                            eRayType::FREEZING_RAY,
                            eCollisionActionType::COLLISION_FINISHED);
                        mLastCollidedActorId = -1;
                    }
                }
            }
        }
    }

    mLineComponent->SetLineBeginWorldSpacePosition(mElectroLineBegin);
    mLineComponent->SetLineEndWorldSpacePosition(mElectroLineEnd);
}

void FreezingRayActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType ownerType,
    const std::shared_ptr<Actor>& spawnerActor)
{
    mDamageDealerType = ownerType;
    mActorWhoSpawnedMeWp = spawnerActor;
    DropState();
    SetIsEnabled(true);
    mActivityState = eMissileActivityState::ACTIVE;
}

void FreezingRayActor::TriggerExplosion()
{
    mActivityState = eMissileActivityState::EXPLOSION;
    TriggerExplosionFinished();
}

void FreezingRayActor::TriggerExplosionFinished()
{
    mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
    TriggerDisabled();
}

void FreezingRayActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;
    DropState();
    SetIsEnabled(false);
}

std::shared_ptr<MissileExplosionVisitorBase> FreezingRayActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<FreezingRayExplosionVisitor>(std::static_pointer_cast<FreezingRayActor>(shared_from_this()));
}

void FreezingRayActor::SetLineComponent(const std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent>& lineComponent)
{
    mLineComponent = lineComponent;
}

void FreezingRayActor::DropState()
{
}

std::weak_ptr<Actor> FreezingRayActor::GetActorWhoSpawnedMeWp() const
{
    return mActorWhoSpawnedMeWp;
}
} // namespace Game
