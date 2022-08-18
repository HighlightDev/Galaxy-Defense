#include "CombatController.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/LoggerExtension.h"

#include "Implementation/Factories/WeakSpaceShipFactory.h"
#include "Implementation/Factories/WeakMissileFactory.h"
#include "Implementation/Factories/BlackHoleMissileFactory.h"
#include "Implementation/Controllers/SpaceShipPlayerController.h"
#include "Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h"

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;
using namespace Resources;

namespace Game
{
    float mDeltaTime = 0.0f;

    CombatController::CombatController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mEnemies(),
          mLevelBounds(BoundingBox(glm::vec3(0), glm::vec3(50, 50, 100)))
    {
        MainPlayerActionEvent::GetInstance()->AddListener(this);
        PhysicsCollisionEvent::GetInstance()->AddListener(this);
    }

    CombatController::~CombatController()
    {
        MainPlayerActionEvent::GetInstance()->RemoveListener(this);
        PhysicsCollisionEvent::GetInstance()->RemoveListener(this);
    }

    void CombatController::OnPreLevelInit()
    {
    }

    void CombatController::OnLevelInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            CreateWeaponBulletPool(3, sceneSp);

            WeakSpaceShipFactory spaceShipFactory;

            for (size_t i = 0; i < 2; ++i)
            {
                const auto &a_enemyShip = spaceShipFactory.CreateSpaceShip(sceneSp,
                                                                           glm::vec3(),
                                                                           glm::vec3(),
                                                                           glm::vec3(9));

                mEnemies.emplace_back(a_enemyShip);
            }
        }
    }

    void CombatController::OnPostLevelInit()
    {
    }

    void CombatController::PostPlayLevelFinished()
    {
        for (const auto &missile : mMissilesPool)
        {
            missile->SetIsEnabled(false); // disable all missiles at level start
        }

        static constexpr float x_axisHalfWidth = 20.0f;
        static constexpr float y_axisHalfHeight = 20.0f;
        for (const auto &spaceship : mEnemies)
        {
            const float x = (Random::Float() * 2.0f) - 1.0f;
            glm::vec3 startPosition((x_axisHalfWidth * x), 0.0f, 100.0f);
            spaceship->TriggerSpawn(startPosition);
        }
    }

    void CombatController::ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data)
    {
        const auto &playerAction = std::get<0>(data);

        if (eMainPlayerActionEnum::SHOOT == playerAction && !bIsCoolDownInProgress)
        {
            bIsCoolDownInProgress = true;
            const auto &playerAction = std::get<0>(data);
            if (const auto &sceneSp = mScene.lock())
            {
                ShootBullet(mPlayerShip->GetRootComponent()->GetTranslation());
            }
        }
    }

    void CombatController::ProcessEvent(const typename PhysicsCollisionEvent::EventData_t &data)
    {
        const ePhysicsCollisionEventType collisionEventType = std::get<0>(data);
        const ePhysicsBodyType physBodyType = std::get<1>(data);
        const auto this_actor_id = std::get<3>(data);
        const auto that_actor_id = std::get<5>(data);

        if (ePhysicsBodyType::GHOST == physBodyType)
        {
            if (const auto &sceneSp = mScene.lock())
            {
                const auto &a_thisEnemyShipOwnerActorIt = FindEnemyShipOwnerActorById(this_actor_id);
                auto a_thisBulletOwnerActorIt = FindBulletOwnerActorById(this_actor_id);

                const auto &a_thatEnemyShipOwnerActorIt = FindEnemyShipOwnerActorById(that_actor_id);
                auto a_thatBulletOwnerActorIt = FindBulletOwnerActorById(that_actor_id);

                std::shared_ptr<MissileActor> ownerMissileActor;
                std::shared_ptr<SpaceshipActor> ownerEnemyShipActor;
                uint64_t missileActor_id;
                uint64_t spaceshipActor_id;

                if (a_thisBulletOwnerActorIt != mMissilesPool.end() || a_thatBulletOwnerActorIt != mMissilesPool.end())
                {
                    if (a_thisBulletOwnerActorIt != mMissilesPool.end())
                    {
                        ownerMissileActor = *a_thisBulletOwnerActorIt;
                        missileActor_id = this_actor_id;
                    }
                    else
                    {
                        ownerMissileActor = *a_thatBulletOwnerActorIt;
                        missileActor_id = that_actor_id;
                    }
                }

                if (a_thisEnemyShipOwnerActorIt != mEnemies.end() || a_thatEnemyShipOwnerActorIt != mEnemies.end())
                {
                    if (a_thisEnemyShipOwnerActorIt != mEnemies.end())
                    {
                        ownerEnemyShipActor = *a_thisEnemyShipOwnerActorIt;
                        spaceshipActor_id = this_actor_id;
                    }
                    else
                    {
                        ownerEnemyShipActor = *a_thatEnemyShipOwnerActorIt;
                        spaceshipActor_id = that_actor_id;
                    }
                }

                if (ownerMissileActor && ownerEnemyShipActor)
                {
                    const auto &concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
                                                           ? ownerMissileActor
                                                           : ownerMissileActor->GetChildByObjectId(missileActor_id);

                    const auto &concreteSpaceshipActor = ownerEnemyShipActor->GetObjectId() == spaceshipActor_id
                                                             ? ownerEnemyShipActor
                                                             : ownerEnemyShipActor->GetChildByObjectId(spaceshipActor_id);

                    const std::string collisionType = ePhysicsCollisionEventType::COLLISION_REGISTERED == collisionEventType
                                                          ? "COLLISION_REGISTERED"
                                                          : "COLLISION_UNREGISTER";
                    LogInfo("CombatController::PhysicsCollisionEvent =>",
                            collisionType,
                            "this_actor = ",
                            concreteMissileActor->GetName(),
                            " that_actor = ",
                            concreteSpaceshipActor->GetName());

                    const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
                    if (ePhysicsCollisionEventType::COLLISION_REGISTERED == collisionEventType)
                    {
                        explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor, concreteSpaceshipActor);
                    }
                    else
                    {
                        explosionVisitor->EndExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor, concreteSpaceshipActor);
                    }
                }
            }
        }
    }

    void CombatController::SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController)
    {
        assert(mainPlayerActorController);
        mMainPlayerActorController = mainPlayerActorController;
        mMainPlayerActorController->SetLevelBounds(mLevelBounds);
        mPlayerShip = mMainPlayerActorController->GetBindedActor();
        assert(mPlayerShip);
    }

    void CombatController::Tick(const float deltaTime)
    {
        // Test bullets if they are still inside level bounds
        FlushToPoolUsedBullets();

        if (bIsCoolDownInProgress)
        {
            mDeltaTime += deltaTime;

            if (mDeltaTime > mCoolDownTime)
            {
                mDeltaTime = 0.0f;
                bIsCoolDownInProgress = false;
            }
        }

        for (const auto &enemyActor : mEnemies)
        {
            if (enemyActor->GetSpaceshipActivityState() == eSpaceshipActivityState::ACTIVE)
            {
                const auto &enemyTranslation = enemyActor->GetRootComponent()->GetTranslation();
                if (enemyTranslation.z < -10.0f)
                {
                    enemyActor->TriggerDisabled();
                }
            }
            else
            {
                static constexpr float x_axisHalfWidth = 20.0f;
                static constexpr float y_axisHalfHeight = 20.0f;
                const float x = (Random::Float() * 2.0f) - 1.0f;
                glm::vec3 startPosition((x_axisHalfWidth * x), 0.0f, 100.0f);
                enemyActor->TriggerSpawn(startPosition);
            }
        }
    }

    void CombatController::CreateWeaponBulletPool(const size_t poolSize, const std::shared_ptr<Scene> &sceneSp)
    {
        BlackHoleMissileFactory bulletFactory;
        for (size_t i = 0; i < poolSize; ++i)
        {
            const auto &a_shipBullet = bulletFactory.CreateWeaponBullet(sceneSp,
                                                                        glm::vec3(0),
                                                                        glm::vec3(),
                                                                        glm::vec3(1.0));

            mMissilesPool.emplace_back(a_shipBullet);
        }
    }

    void CombatController::ShootBullet(const glm::vec3 &bulletStartPosition)
    {
        auto idleBulletIt = std::find_if(mMissilesPool.begin(), mMissilesPool.end(), [](const auto &missile)
                                         { return eMissileActivityState::IDLE == missile->GetMissileActivityState(); });

        if (idleBulletIt == mMissilesPool.end())
        {
            LogInfo("CombatController::ShootBullet => Error - no idle bullets in the pool");
            return;
        }

        (*idleBulletIt)->TriggerSpawn(bulletStartPosition);
    }

    void CombatController::FlushToPoolUsedBullets()
    {
        for (auto &missile : mMissilesPool)
        {
            if (eMissileActivityState::ACTIVE == missile->GetMissileActivityState())
            {
                if (!missile->IsInsideLevel(mLevelBounds))
                {
                    missile->TriggerDisabled();
                }
            }
        }
    }

    typename std::vector<std::shared_ptr<SpaceshipActor>>::iterator
    CombatController::FindEnemyShipByName(const std::string &actorName)
    {
        auto foundIt = std::find_if(mEnemies.begin(), mEnemies.end(),
                                    [&actorName = static_cast<const std::string &>(actorName)](const auto &enemyActor)
                                    {
                                        return actorName == enemyActor->GetName();
                                    });
        return foundIt;
    }

    typename std::vector<std::shared_ptr<SpaceshipActor>>::iterator
    CombatController::FindEnemyShipOwnerActorById(const uint64_t actorId)
    {
        auto foundIt = std::find_if(mEnemies.begin(), mEnemies.end(), [=](const auto &enemyActor)
                                    { return enemyActor->HasGameObjectIdInHierarchy(actorId); });
        return foundIt;
    }

    typename std::vector<std::shared_ptr<MissileActor>>::iterator
    CombatController::FindBulletByName(const std::string &actorName)
    {
        auto foundIt = std::find_if(mMissilesPool.begin(),
                                    mMissilesPool.end(),
                                    [&actorName = static_cast<const std::string &>(actorName)](const auto &missile)
                                    {
                                        return actorName == missile->GetName();
                                    });
        return foundIt;
    }

    typename std::vector<std::shared_ptr<MissileActor>>::iterator
    CombatController::FindBulletOwnerActorById(const uint64_t actorId)
    {
        auto foundIt = std::find_if(mMissilesPool.begin(),
                                    mMissilesPool.end(),
                                    [=](const auto &missile)
                                    {
                                        return missile->HasGameObjectIdInHierarchy(actorId);
                                    });
        return foundIt;
    }
}