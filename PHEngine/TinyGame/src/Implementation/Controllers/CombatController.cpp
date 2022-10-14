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
#include "Implementation/Factories/BackgroundPlanetsFactory.h"
#include "Implementation/Controllers/SpaceShipPlayerController.h"
#include "Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h"
#include "Implementation/SpaceSceneCamera.h"

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
          mLevelBounds(BoundingBox(glm::vec3(0), glm::vec3(50, 50, 100))),
          mCameraVisibilityArea()
    {
        MainPlayerActionEvent::GetInstance()->AddListener(this);
        PhysicsCollisionEvent::GetInstance()->AddListener(this);

        mBackgroundPlanetsSpawnTimer.SetIntervalMs(2000);
        mBackgroundPlanetsSpawnTimer.SetIsRepeat(true);
        mBackgroundPlanetsSpawnTimer.SetCallback(std::bind(&CombatController::OnBackgroundPlanetsSpawnTimerTimeout, this));
    }

    CombatController::~CombatController()
    {
        MainPlayerActionEvent::GetInstance()->RemoveListener(this);
        PhysicsCollisionEvent::GetInstance()->RemoveListener(this);
    }

    void CombatController::OnPreLevelInit()
    {
    }

    void CombatController::OnCameraTransformChanged(ACamera *eventSrc)
    {
        const auto &nForwardDir = glm::normalize(eventSrc->GetEyeSpaceForwardVector());
        const auto &eyePos = eventSrc->GetEyeVector();

        const auto planePosition = eyePos + nForwardDir * 100.0f;
        const auto planeNormal = glm::vec3(0, 1, 0);
        glm::vec4 backgroundPlane = glm::vec4(planeNormal, -glm::dot(planeNormal, planePosition));

        const auto cameraFrustumOpt = eventSrc->GetCameraFrustum();

        if (cameraFrustumOpt)
        {
            const auto cameraFrustum = cameraFrustumOpt.value();
            const auto leftPlane = cameraFrustum.GetPlaneByName(eFrustumPlaneName::Left);
            const auto rightPlane = cameraFrustum.GetPlaneByName(eFrustumPlaneName::Right);
            const auto topPlane = cameraFrustum.GetPlaneByName(eFrustumPlaneName::Top);
            const auto bottomPlane = cameraFrustum.GetPlaneByName(eFrustumPlaneName::Bottom);

            auto ltb = EngineMath::TestPlaneToPlaneToPlane(leftPlane, topPlane, backgroundPlane);
            auto lbb = EngineMath::TestPlaneToPlaneToPlane(leftPlane, bottomPlane, backgroundPlane);
            auto rtb = EngineMath::TestPlaneToPlaneToPlane(rightPlane, topPlane, backgroundPlane);
            auto rbb = EngineMath::TestPlaneToPlaneToPlane(rightPlane, bottomPlane, backgroundPlane);

            if (ltb.has_value() && lbb.has_value() && rtb.has_value() && rbb.has_value())
            {
                glm::vec3 ltbPos = ltb.value();
                glm::vec3 lbbPos = lbb.value();
                glm::vec3 rtbPos = rtb.value();
                glm::vec3 rbbPos = rbb.value();

                float minX, maxX, minZ, maxZ;
                float y = planePosition.y;
                minX = glm::min(ltbPos.x, glm::min(lbbPos.x, glm::min(rtbPos.x, rbbPos.x)));
                maxX = glm::max(ltbPos.x, glm::max(lbbPos.x, glm::max(rtbPos.x, rbbPos.x)));
                minZ = glm::min(ltbPos.z, glm::min(lbbPos.z, glm::min(rtbPos.z, rbbPos.z)));
                maxZ = glm::max(ltbPos.z, glm::max(lbbPos.z, glm::max(rtbPos.z, rbbPos.z)));

                glm::vec3 origin(minX + ((maxX - minX) * 0.5f), y, minZ + ((maxZ - minZ) * 0.5f));
                glm::vec3 halfExtent((maxX - minX) * 0.5f, 350.0f, (maxZ - minZ) * 0.5f);

                mCameraVisibilityArea = std::make_unique<BoundingBox>(origin, halfExtent);

                const auto &nRightDir = glm::normalize(eventSrc->GetEyeSpaceRightVector());
                const auto nUpDir = glm::normalize(glm::cross(nRightDir, nForwardDir));
                for (const auto &backgrounObject : mBackgroundSpaceObjects)
                {
                    backgrounObject->GetMovementComponent()->SetDirection(nUpDir);
                }
            }
        }
    }

    void CombatController::OnLevelInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            const auto &spaceCamera = std::static_pointer_cast<SpaceSceneCamera>(sceneSp->GetMainCamera());
            spaceCamera->AddCameraTransformObserver(this);

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

            CreateBackgroundSpaceObjectsPool(sceneSp);
        }
    }

    void CombatController::CreateBackgroundSpaceObjectsPool(const std::shared_ptr<Scene> &sceneSp)
    {
        BackgroundPlanetsFactory factory;

        mBackgroundSpaceObjects.emplace_back(factory.CreateSpaceObject(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1), "planet_1.png", 15.0f));
        mBackgroundSpaceObjects.emplace_back(factory.CreateSpaceObject(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1), "planet_2.png", 5.0f));
        mBackgroundSpaceObjects.emplace_back(factory.CreateSpaceObject(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1), "planet_3.png", 7.0f));
        mBackgroundSpaceObjects.emplace_back(factory.CreateSpaceObject(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1), "planet_4.png", 10.0f));
        mBackgroundSpaceObjects.emplace_back(factory.CreateSpaceObject(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1), "planet_5.png", 12.0f));
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

        for (const auto &backgroundObject : mBackgroundSpaceObjects)
        {
            backgroundObject->TriggerDisabled();
        }

        mBackgroundPlanetsSpawnTimer.StartTimer();
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

        if (mCameraVisibilityArea)
        {
            for (const auto &backgroundObject : mBackgroundSpaceObjects)
            {
                if (eSpaceObjectActivityState::ACTIVE == backgroundObject->GetActivityState() && !backgroundObject->IsInsideLevel(*mCameraVisibilityArea))
                {
                    backgroundObject->TriggerDisabled();
                }
            }
        }
    }

    void CombatController::OnBackgroundPlanetsSpawnTimerTimeout()
    {
        if (mCameraVisibilityArea)
        {
            const auto randomValue = Random::Float();
            const bool bShouldSpawnBackground = (randomValue >= 0.4f && randomValue <= 0.5f);

            static constexpr auto minSpeed = 5.0f, maxSpeed = 15.0f;
            static constexpr auto minSize = 3.0f, maxSize = 20.0f;

            if (bShouldSpawnBackground)
            {
                const float x = mCameraVisibilityArea->GetOrigin().x + mCameraVisibilityArea->GetHalfExtent().x * ((2.0f * Random::Float()) - 1.0f);
                const auto foundFree = std::find_if(mBackgroundSpaceObjects.begin(), mBackgroundSpaceObjects.end(), [](const auto &object)
                                                    { return object->GetActivityState() == eSpaceObjectActivityState::IDLE; });
                if (foundFree != mBackgroundSpaceObjects.end())
                {
                    const float speed = (Random::Float() * (maxSpeed - minSpeed)) + minSpeed;
                    const float size = (Random::Float() * (maxSize - minSize)) + minSize;
                    const auto& foundPlanet = *foundFree;
                    foundPlanet->TriggerSpawn(glm::vec3(x, mCameraVisibilityArea->GetOrigin().y, mCameraVisibilityArea->GetMax().z));
                    foundPlanet->GetMovementComponent()->SetReferenceSpeed(speed);
                    foundPlanet->GetMovementComponent()->SetCurrentSpeedToReferenceValue();
                    foundPlanet->SetBillboardExtentSize(size);
                    LogInfo("CombatController::OnBackgroundPlanetsSpawnTimerTimeout => Spawn background planet. Object Id: ", (*foundFree)->GetObjectId());
                }
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

    glm::vec3 CombatController::GetRandomPositionForBackgroundSpaceObject() const
    {
        return glm::vec3(mCameraVisibilityArea->GetOrigin().x, mCameraVisibilityArea->GetOrigin().y, mCameraVisibilityArea->GetMax().z);
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