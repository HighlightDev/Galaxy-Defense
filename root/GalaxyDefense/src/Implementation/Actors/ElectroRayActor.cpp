#include "ElectroRayActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/DynamicBeamComponent.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/RayCastWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Events/ShootRayCollisionEvent.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/MissileExplosionVisitors/ElectroRayExplosionVisitor.h"

#include <utility>

using namespace EnginePhysics;
using namespace Event;

namespace Game {
ElectroRayActor::ElectroRayActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler)
    : MissileActor(gameObjectName, rootComponent, combatActorsPoolHandler)
    , mLineComponent()
    , mSpaceshipWhoSpawnedMeWp()
    , mElectroLineBegin()
    , mElectroLineEnd()
    , mElectroLineOriginSpeed(1.0f)
    , mElectroLineDestinationSpeed(1.5f)
    , mElectroLineOriginStartMovementDelayTimer(std::make_shared<GameThreadTimer>())
    , bLineOriginStartMovement(false)
    , bElectroLineCollided(false)
    , mCollidedSpaceship()
    , mOpacity(std::make_shared<EngineObjectProperty<float>>(1.0f, "p_opacity"))
{
    mMissileType = eMissileType::ELECTRO_RAY;
    AddEngineProperty(mOpacity);
    Initialize();
}

void ElectroRayActor::Initialize()
{
    mElectroLineOriginStartMovementDelayTimer->Initialize();
    mElectroLineOriginStartMovementDelayTimer->SetIsPausable(true);
    mElectroLineOriginStartMovementDelayTimer->SetIsRepeat(false);
    mElectroLineOriginStartMovementDelayTimer->SetIntervalMs(1000);
}

void ElectroRayActor::OnSceneOwnerInitialized()
{
    Actor::OnSceneOwnerInitialized();
    mElectroLineOriginStartMovementDelayTimer->SetCallback(
        [weak_me = std::weak_ptr<ElectroRayActor>(std::static_pointer_cast<ElectroRayActor>(shared_from_this()))]() {
            if (auto shared_me = weak_me.lock()) {
                shared_me->OnElectroLineOriginStartMovementDelayTimerTimeout();
            }
        });
}

bool ElectroRayActor::IsInsideLevel(const BoundingBox3D& boundingBox) const
{
    return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mElectroLineBegin)
        || EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), mElectroLineBegin);
}

void ElectroRayActor::Tick(const float deltaTimeSec, const float playSpeed)
{
    MissileActor::Tick(deltaTimeSec, playSpeed);

    ext_assert(mLineComponent, "ElectroRayActor line component is null");

    glm::vec3 electroLineDirection = mElectroLineDirection;

    if (!bElectroLineCollided) {
        mElectroLineEnd += mElectroLineDirection * mElectroLineDestinationSpeed * deltaTimeSec * playSpeed;

        if (const auto& sceneSp = mSceneOwner.lock()) {
            if (const auto& spaceshipWhoSpawnedMeSp = mSpaceshipWhoSpawnedMeWp.lock()) {
                std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionPhysComponents;
                const auto& spaceStations = mCombatActorsPoolHandler->GetSpaceStationActors();
                excludeCollisionPhysComponents.reserve(
                    spaceStations.size() + mCombatActorsPoolHandler->GetMissileActors().size() + 1);
                if (const auto& ownerSpaceshipPhysComp = spaceshipWhoSpawnedMeSp->GetPhysicsComponent()) {
                    excludeCollisionPhysComponents.emplace_back(ownerSpaceshipPhysComp);
                }
                const auto& spaceStationsPhysComponents = mCombatActorsPoolHandler->GetSpaceStationsPhysicsComponents();
                const auto& missilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(
                    {eMissileType::BOMB, eMissileType::FREEZING_BOMB, eMissileType::BLACK_HOLE, eMissileType::PLASMA_BOMB});
                excludeCollisionPhysComponents.insert(
                    excludeCollisionPhysComponents.end(), spaceStationsPhysComponents.begin(), spaceStationsPhysComponents.end());
                excludeCollisionPhysComponents.insert(
                    excludeCollisionPhysComponents.end(), missilePhysComponents.begin(), missilePhysComponents.end());

                auto rayWithoutSpawnSpaceship = RayCastWithFilterAdapter(excludeCollisionPhysComponents);
                rayWithoutSpawnSpaceship.RayTest(sceneSp->GetPhysicsWorld(), mElectroLineBegin, mElectroLineEnd);
                if (rayWithoutSpawnSpaceship.IsRayHitCollision()) {
                    if (const auto& collidedPhysDescriptor = rayWithoutSpawnSpaceship.GetCollisionHitPhysicsDescriptor()) {
                        const auto& collidedActor = sceneSp->GetActorById(collidedPhysDescriptor->GetOwnerActorEngineObjectId());
                        mElectroLineEnd = collidedActor->GetRootComponent()->GetTranslation();
                        mCollidedSpaceship = collidedActor;
                        bElectroLineCollided = true;
                        ShootRayCollisionEvent::GetInstance()->SendEvent(
                            eExecutionOrder::POST_EXECUTION,
                            std::static_pointer_cast<MissileActor>(shared_from_this()),
                            collidedActor->shared_from_this(),
                            eRayType::ELECTRO_RAY,
                            eCollisionActionType::COLLISION_STARTED);
                    }
                }
            }
        }
    } else if (const auto& collidedSpaceShipSp = mCollidedSpaceship.lock()) {
        if (collidedSpaceShipSp->IsEnabled() && collidedSpaceShipSp->IsVisible()) {
            mElectroLineEnd = collidedSpaceShipSp->GetRootComponent()->GetTranslation();
            electroLineDirection = glm::normalize(mElectroLineEnd - mElectroLineBegin);
        } else {
            mCollidedSpaceship.reset();
            TriggerDisabled();
            return;
        }
    } else {
        TriggerDisabled();
        return;
    }

    if (bLineOriginStartMovement) {
        mElectroLineBegin += electroLineDirection * mElectroLineOriginSpeed * deltaTimeSec * playSpeed;
        if (bElectroLineCollided
            && (EngineMath::CheckSimilarityVec3(mElectroLineBegin, mElectroLineEnd)
                || (glm::dot(mElectroLineBegin, mElectroLineDirection) >= glm::dot(mElectroLineEnd, mElectroLineDirection)))) {
            TriggerDisabled();
        }
    }

    mLineComponent->SetStartWorldPosition(mElectroLineBegin);
    mLineComponent->SetEndWorldPosition(mElectroLineEnd);
}

void ElectroRayActor::TriggerSpawn(
    const glm::vec3& position,
    const glm::vec3& direction,
    const float yawDegrees,
    const eDamageDealerType ownerType,
    const std::shared_ptr<SpaceStationActor>& spawnerActor)
{
    mDamageDealerType = ownerType;
    mSpaceshipWhoSpawnedMeWp = spawnerActor;
    DropState();
    SetIsEnabled(true);
    mActivityState = eMissileActivityState::ACTIVE;
    mElectroLineDirection = direction;

    if (const auto& spaceshipWhoSpawnedMeSp = mSpaceshipWhoSpawnedMeWp.lock()) {
        mElectroLineEnd = mElectroLineBegin = spaceshipWhoSpawnedMeSp->GetRootComponent()->GetTranslation();
        mElectroLineOriginStartMovementDelayTimer->StartTimer();
    }
}

void ElectroRayActor::TriggerExplosion()
{
    mActivityState = eMissileActivityState::EXPLOSION;
    TriggerExplosionFinished();
}

void ElectroRayActor::TriggerExplosionFinished()
{
    mActivityState = eMissileActivityState::EXPLOSION_FINISHED;
    TriggerDisabled();
}

void ElectroRayActor::TriggerDisabled()
{
    mActivityState = eMissileActivityState::IDLE;
    DropState();
    SetIsEnabled(false);
}

std::shared_ptr<MissileExplosionVisitorBase> ElectroRayActor::CreateMissileExplosionVisitor()
{
    return std::make_shared<ElectroRayExplosionVisitor>(std::static_pointer_cast<ElectroRayActor>(shared_from_this()));
}

void ElectroRayActor::SetLineComponent(const std::shared_ptr<::EngineCore::DynamicBeamComponent>& lineComponent)
{
    mLineComponent = lineComponent;
}

void ElectroRayActor::SetElectroLineOriginSpeed(const float speed)
{
    mElectroLineOriginSpeed = speed;
}

void ElectroRayActor::SetElectroLineDestinationSpeed(const float speed)
{
    mElectroLineDestinationSpeed = speed;
}

void ElectroRayActor::OnElectroLineOriginStartMovementDelayTimerTimeout()
{
    bLineOriginStartMovement = true;
}

void ElectroRayActor::DropState()
{
    mElectroLineOriginStartMovementDelayTimer->StopTimer();
    mCollidedSpaceship.reset();
    bLineOriginStartMovement = false;
    bElectroLineCollided = false;
    mElectroLineDirection = glm::vec3(0.0f);
    if (const auto& spaceshipWhoSpawnedMeSp = mSpaceshipWhoSpawnedMeWp.lock()) {
        const bool bSpaceshipIsEnabledAndVisible = spaceshipWhoSpawnedMeSp->IsEnabled() && spaceshipWhoSpawnedMeSp->IsVisible();
        if (bSpaceshipIsEnabledAndVisible) {
            mElectroLineEnd = mElectroLineBegin = spaceshipWhoSpawnedMeSp->GetRootComponent()->GetTranslation();
        } else {
            mElectroLineEnd = mElectroLineBegin = glm::vec3(0.0f);
            mSpaceshipWhoSpawnedMeWp.reset();
        }
    } else {
        mElectroLineEnd = mElectroLineBegin = glm::vec3(0.0f);
    }
}
} // namespace Game