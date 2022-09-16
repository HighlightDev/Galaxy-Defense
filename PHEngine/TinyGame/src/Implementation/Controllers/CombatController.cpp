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
#include "Implementation/Factories/BombMissileFactory.h"
#include "Implementation/Factories/FreezingMissileFactory.h"
#include "Implementation/Factories/BlackHoleMissileFactory.h"
#include "Implementation/Factories/AsteroidFactory.h"
#include "Implementation/Controllers/SpaceShipPlayerController.h"
#include "Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h"

#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"

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

    void CreateBillboard(const std::shared_ptr<Scene> &sceneSp);

    void CombatController::OnLevelInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            CreateWeaponBulletPool(sceneSp);

            WeakSpaceShipFactory spaceShipFactory;

            for (size_t i = 0; i < 2; ++i)
            {
                const auto &a_enemyShip = spaceShipFactory.CreateSpaceShip(sceneSp,
                                                                           glm::vec3(),
                                                                           glm::vec3(),
                                                                           glm::vec3(9));

                mEnemies.emplace_back(a_enemyShip);
            }

            CreateAsteroidsPool(sceneSp);

            CreateBillboard(sceneSp);
        }
    }

    void CreateBillboard(const std::shared_ptr<Scene> &sceneSp)
    {
        const auto billboardActor = std::make_shared<Actor>("Billboard Actor", std::make_shared<SceneComponent>("BillboardActor_rootComponent",
                                                                                                                glm::vec3(0), glm::vec3(), glm::vec3(1)));

        auto billboardComponentCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
        BillboardComponentData data("c_billboardMesh", 25.0f, "planet_1.png", glm::vec3(0.0f, -100.0f, 80.0f), glm::vec3(1.0f));
        const auto &billboardComponent = sceneSp->CreateComponent_GameThread(billboardComponentCreator, data);
        billboardActor->AddComponent(billboardComponent);

        sceneSp->AddActor(billboardActor);
    }

    void CombatController::OnPostLevelInit()
    {
    }

    void CombatController::PostPlayLevelFinished()
    {
        for (const auto &missile : mMissilesPool)
        {
            missile->SetIsEnabled(false); // disable all missiles at level beginning
        }

        for (const auto &spaceship : mEnemies)
        {
            spaceship->TriggerSpawn(GenRandomPositionForSpaceship());
        }

        for (const auto &asteroid : mSpaceObjectsPool)
        {
            asteroid->TriggerSpawn(GenRandomPositionForSpaceObject());
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
                const auto &a_thisBulletOwnerActorIt = FindBulletOwnerActorById(this_actor_id);
                const auto &a_thisSpaceObjectOwnerActorIt = FindSpaceObjectOwnerActorById(this_actor_id);

                const auto &a_thatEnemyShipOwnerActorIt = FindEnemyShipOwnerActorById(that_actor_id);
                const auto &a_thatBulletOwnerActorIt = FindBulletOwnerActorById(that_actor_id);
                const auto &a_thatSpaceObjectOwnerActorIt = FindSpaceObjectOwnerActorById(that_actor_id);

                std::shared_ptr<MissileActor> ownerMissileActor;
                std::shared_ptr<SpaceshipActor> ownerEnemyShipActor;
                std::shared_ptr<SpaceObjectActor> ownerSpaceObjectActor;
                uint64_t missileActor_id;
                uint64_t spaceshipActor_id;
                uint64_t spaceObjectActor_id;

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

                if (a_thisSpaceObjectOwnerActorIt != mSpaceObjectsPool.end() || a_thatSpaceObjectOwnerActorIt != mSpaceObjectsPool.end())
                {
                    if (a_thisSpaceObjectOwnerActorIt != mSpaceObjectsPool.end())
                    {
                        ownerSpaceObjectActor = *a_thisSpaceObjectOwnerActorIt;
                        spaceObjectActor_id = this_actor_id;
                    }
                    else
                    {
                        ownerSpaceObjectActor = *a_thatSpaceObjectOwnerActorIt;
                        spaceObjectActor_id = that_actor_id;
                    }
                }

                const std::string collisionType = ePhysicsCollisionEventType::COLLISION_REGISTERED == collisionEventType
                                                      ? "collision registered"
                                                      : "collision unregister";

                if (ownerMissileActor && ownerSpaceObjectActor)
                {
                    const auto &concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
                                                           ? ownerMissileActor
                                                           : ownerMissileActor->GetChildByObjectId(missileActor_id);

                    LogInfo("CombatController::PhysicsCollisionEvent => ", collisionType, "missile with space object, this_actor = ", concreteMissileActor->GetName(),
                            " that_actor = ",
                            ownerSpaceObjectActor->GetName());

                    const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
                    if (ePhysicsCollisionEventType::COLLISION_REGISTERED == collisionEventType)
                    {
                        explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor, ownerSpaceObjectActor);
                    }
                    else
                    {
                        explosionVisitor->EndExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor, ownerSpaceObjectActor);
                    }
                }
                else if (ownerEnemyShipActor && ownerSpaceObjectActor)
                {
                    LogInfo("CombatController::PhysicsCollisionEvent =>", collisionType, "spaceship with space object, this_actor = ", ownerEnemyShipActor->GetName(),
                            " that_actor = ", ownerSpaceObjectActor->GetName());

                    ownerEnemyShipActor->TriggerDamageReceived(1UL);
                    ownerSpaceObjectActor->TriggerDisabled();
                }
                else if (ownerMissileActor && ownerEnemyShipActor)
                {
                    const auto &concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
                                                           ? ownerMissileActor
                                                           : ownerMissileActor->GetChildByObjectId(missileActor_id);

                    const auto &concreteSpaceshipActor = ownerEnemyShipActor->GetObjectId() == spaceshipActor_id
                                                             ? ownerEnemyShipActor
                                                             : ownerEnemyShipActor->GetChildByObjectId(spaceshipActor_id);

                    LogInfo("CombatController::PhysicsCollisionEvent =>", collisionType, "missile with spaceship, this_actor = ", concreteMissileActor->GetName(), " that_actor = ",
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
            if (eSpaceshipActivityState::ACTIVE == enemyActor->GetSpaceshipActivityState())
            {
                const auto &enemyTranslation = enemyActor->GetRootComponent()->GetTranslation();
                if (enemyTranslation.z < -10.0f)
                {
                    enemyActor->TriggerDisabled();
                }
            }
            else
            {
                enemyActor->TriggerSpawn(GenRandomPositionForSpaceship());
            }
        }

        for (const auto &spaceObject : mSpaceObjectsPool)
        {
            if (eSpaceObjectActivityState::ACTIVE == spaceObject->GetActivityState())
            {
                const auto &asteroidTranslation = spaceObject->GetRootComponent()->GetTranslation();
                if (asteroidTranslation.x > 50.0f)
                {
                    spaceObject->TriggerDisabled();
                }
            }
            else
            {
                spaceObject->TriggerSpawn(GenRandomPositionForSpaceObject());
            }
        }
    }

    glm::vec3 CombatController::GenRandomPositionForSpaceObject() const
    {
        static constexpr float x_axisHalfWidth = 20.0f;
        static constexpr float y_axisHalfHeight = 20.0f;
        static constexpr float min_squared_radius = 150.0f;

        glm::vec3 potentialPosition(0.0f);
        do
        {
            const float x = Random::Float() * 50.0f + 100.0f;
            const float z = (Random::Float() * 2.0f) - 1.0f;
            potentialPosition = glm::vec3(-x, 0.0f, (30.0f + x_axisHalfWidth * z));
        } while (std::any_of(mSpaceObjectsPool.begin(), mSpaceObjectsPool.end(), [=](const auto &existingSpaceObject)
                             { return ((eSpaceObjectActivityState::ACTIVE == existingSpaceObject->GetActivityState()) && ((glm::length2(existingSpaceObject->GetWorldPosition() - potentialPosition) < min_squared_radius))); }));

        return potentialPosition;
    }

    glm::vec3 CombatController::GenRandomPositionForSpaceship() const
    {
        static constexpr float x_axisHalfWidth = 20.0f;
        static constexpr float z_axisHalfHeight = 20.0f;
        static constexpr float min_squared_radius = 250.0f;

        glm::vec3 potentialPosition(0.0f);
        do
        {
            const float x = ((Random::Float() * 2.0f) - 1.0f) * x_axisHalfWidth;
            const float z = Random::Float() * 25.0f + 75.0f;
            potentialPosition = glm::vec3(x, 0.0f, z);
        } while (std::any_of(mEnemies.begin(), mEnemies.end(), [=](const auto &existingEnemy)
                             { return ((eSpaceshipActivityState::ACTIVE == existingEnemy->GetSpaceshipActivityState()) && ((glm::length2(existingEnemy->GetWorldPosition() - potentialPosition) < min_squared_radius))); }));

        return potentialPosition;
    }

    void CombatController::CreateWeaponBulletPool(const std::shared_ptr<Scene> &sceneSp)
    {
        FreezingMissileFactory freezingMissileFactory;
        for (size_t i = 0; i < 1; ++i)
        {
            const auto &a_missile = freezingMissileFactory.CreateMissile(sceneSp,
                                                                         glm::vec3(0),
                                                                         glm::vec3(),
                                                                         glm::vec3(1.0));

            mMissilesPool.emplace_back(a_missile);
        }

        BombMissileFactory bombMissileFactory;
        for (size_t i = 0; i < 3; ++i)
        {
            const auto &a_missile = bombMissileFactory.CreateMissile(sceneSp,
                                                                     glm::vec3(0),
                                                                     glm::vec3(),
                                                                     glm::vec3(1.0));

            mMissilesPool.emplace_back(a_missile);
        }

        BlackHoleMissileFactory blackHoleMissileFactory;
        for (size_t i = 0; i < 1; ++i)
        {
            const auto &a_missile = blackHoleMissileFactory.CreateMissile(sceneSp,
                                                                          glm::vec3(0),
                                                                          glm::vec3(),
                                                                          glm::vec3(1.0));

            mMissilesPool.emplace_back(a_missile);
        }
    }

    void CombatController::CreateAsteroidsPool(const std::shared_ptr<Scene> &sceneSp)
    {
        AsteroidFactory asteroidFactory;
        for (size_t i = 0; i < 5; ++i)
        {
            const auto &a_asteroid = asteroidFactory.CreateSpaceObject(sceneSp, glm::vec3(0),
                                                                       glm::vec3(),
                                                                       glm::vec3(40.0));
            mSpaceObjectsPool.emplace_back(a_asteroid);
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

    typename std::vector<std::shared_ptr<SpaceObjectActor>>::iterator
    CombatController::FindSpaceObjectOwnerActorById(const uint64_t actorId)
    {
        auto foundIt = std::find_if(mSpaceObjectsPool.begin(),
                                    mSpaceObjectsPool.end(),
                                    [=](const auto &spaceObject)
                                    {
                                        return spaceObject->HasGameObjectIdInHierarchy(actorId);
                                    });
        return foundIt;
    }
}