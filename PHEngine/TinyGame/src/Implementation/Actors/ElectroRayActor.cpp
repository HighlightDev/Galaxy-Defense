#include "ElectroRayActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/RayCastWithFilterAdapter.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Events/RayCollisionEvent.h"
#include "Implementation/MissileExplosionVisitors/ElectroRayExplosionVisitor.h"

#include <utility>

using namespace EnginePhysics;
using namespace Event;

namespace Game
{
    ElectroRayActor::ElectroRayActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : MissileActor(gameObjectName, rootComponent),
          mLineComponent(),
          mSpaceshipWhoSpawnedMeWp(),
          mElectroLineBegin(),
          mElectroLineEnd(),
          mElectroLineOriginSpeed(1.0f),
          mElectroLineDestinationSpeed(1.5f),
          mElectroLineOriginStartMovementDelayTimer(),
          bLineOriginStartMovement(false),
          bElectroLineCollided(false),
          mCollidedSpaceship()
    {
        Initialize();
    }

    void ElectroRayActor::Initialize()
    {
        mElectroLineOriginStartMovementDelayTimer.SetIsPausable(true);
        mElectroLineOriginStartMovementDelayTimer.SetIsRepeat(false);
        mElectroLineOriginStartMovementDelayTimer.SetIntervalMs(2000);
        mElectroLineOriginStartMovementDelayTimer.SetCallback(std::bind(&ElectroRayActor::OnElectroLineOriginStartMovementDelayTimerTimeout, this));
    }

    bool ElectroRayActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(),
                                           boundingBox.GetMax(),
                                           mElectroLineBegin);
    }

    void ElectroRayActor::Tick(const float deltaTime)
    {
        MissileActor::Tick(deltaTime);

        assert(mLineComponent);

        glm::vec3 electroLineDirection(0.0f, 0.0f, 1.0f);

        if (!bElectroLineCollided)
        {
            mElectroLineEnd += glm::vec3(0.0f, 0.0f, 1.0f) * mElectroLineDestinationSpeed * deltaTime;

            if (const auto &sceneSp = mSceneOwner.lock())
            {
                if (const auto &spaceshipWhoSpawnedMeSp = mSpaceshipWhoSpawnedMeWp.lock())
                {
                    std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionPhysComponents;
                    if (spaceshipWhoSpawnedMeSp->GetPhysicsComponent())
                    {
                        excludeCollisionPhysComponents.emplace_back(spaceshipWhoSpawnedMeSp->GetPhysicsComponent());
                    }

                    auto rayWithoutSpawnSpaceship = RayCastWithFilterAdapter(excludeCollisionPhysComponents);
                    rayWithoutSpawnSpaceship.RayTest(sceneSp->GetPhysicsWorld(), mElectroLineBegin, mElectroLineEnd);
                    if (rayWithoutSpawnSpaceship.IsRayHitCollision())
                    {
                        if (const auto &collidedPhysDescriptor = rayWithoutSpawnSpaceship.GetCollisionHitPhysicsDescriptor())
                        {
                            const auto &collidedActor = sceneSp->GetActorById(collidedPhysDescriptor->GetOwnerActorGameObjectId());
                            mElectroLineEnd = collidedActor->GetRootComponent()->GetTranslation();
                            mCollidedSpaceship = collidedActor;
                            bElectroLineCollided = true;
                            RayCollisionEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                                        std::static_pointer_cast<MissileActor>(shared_from_this()),
                                                                        collidedActor->shared_from_this());
                        }
                    }
                }
            }
        }
        else if (const auto &collidedSpaceShipSp = mCollidedSpaceship.lock())
        {
            mElectroLineEnd = collidedSpaceShipSp->GetRootComponent()->GetTranslation();
            electroLineDirection = glm::normalize(mElectroLineEnd - mElectroLineBegin);
            if (mElectroLineOriginStartMovementDelayTimer.IsRunning())
            {
                mElectroLineOriginStartMovementDelayTimer.StopTimer();
                bLineOriginStartMovement = true;
            }
        }

        if (bLineOriginStartMovement)
        {
            mElectroLineBegin += electroLineDirection * mElectroLineOriginSpeed * deltaTime;
            if (bElectroLineCollided &&
                (EngineMath::CheckSimilarityVec3(mElectroLineBegin, mElectroLineEnd) ||
                 (EngineMath::ProjectVector3OnVector(mElectroLineBegin, glm::vec3(0.0f, 0.0f, 1.0f)) >=
                  EngineMath::ProjectVector3OnVector(mElectroLineEnd, glm::vec3(0.0f, 0.0f, 1.0f)))))
            {
                TriggerDisabled();
            }
        }

        mLineComponent->SetLineBeginWorldSpacePosition(mElectroLineBegin);
        mLineComponent->SetLineEndWorldSpacePosition(mElectroLineEnd);
    }

    void ElectroRayActor::TriggerSpawn(const glm::vec3 &position)
    {
        DropState();
        SetIsEnabled(true);
        mActivityState = eMissileActivityState::ACTIVE;

        if (const auto &spaceshipWhoSpawnedMeSp = mSpaceshipWhoSpawnedMeWp.lock())
        {
            mElectroLineEnd = mElectroLineBegin = spaceshipWhoSpawnedMeSp->GetRootComponent()->GetTranslation();
            mElectroLineOriginStartMovementDelayTimer.StartTimer();
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

    void ElectroRayActor::SetLineComponent(const std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent> &lineComponent)
    {
        mLineComponent = lineComponent;
    }

    void ElectroRayActor::SetSpawnerSpaceship(const std::weak_ptr<::EngineCore::Actor> &spawnerSpaceship)
    {
        mSpaceshipWhoSpawnedMeWp = spawnerSpaceship;
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
        mElectroLineOriginStartMovementDelayTimer.StopTimer();
        mCollidedSpaceship.reset();
        bLineOriginStartMovement = false;
        bElectroLineCollided = false;
        if (const auto &spaceshipWhoSpawnedMeSp = mSpaceshipWhoSpawnedMeWp.lock())
        {
            mElectroLineEnd = mElectroLineBegin = spaceshipWhoSpawnedMeSp->GetRootComponent()->GetTranslation();
        }
    }
}