#include "CombatController.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/CommonCore/JsonHelper.h"

#include "Implementation/Factories/WeakSpaceShipFactory.h"
#include "Implementation/Factories/BombMissileFactory.h"
#include "Implementation/Factories/FreezingMissileFactory.h"
#include "Implementation/Factories/BlackHoleMissileFactory.h"
#include "Implementation/Factories/AsteroidFactory.h"
#include "Implementation/Factories/ElectroRayFactory.h"
#include "Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h"
#include "Implementation/Modifiers/ElectroRayChainModifier.h"
#include "Implementation/GalaxySceneCamera.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/MissileType.h"
#include "Implementation/Levels/LevelSerializationHelper.h"
#include "Implementation/Navigation/PathSegment.h"
#include "Implementation/Navigation/Path.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include <array>
#include <unordered_map>
#include <tuple>

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;
using namespace Resources;

#undef min
#undef max

namespace Game
{
    float mDeltaTime = 0.0f;

    CombatController::CombatController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mEnemies(),
          mLevelBounds(BoundingBox3D(glm::vec3(0), glm::vec3(100, 50, 100))),
          mElectroRayChainActorPool(std::make_shared<ElectroRayChainActorPool>(scene)),
          mNavigationController(std::make_shared<NavigationController>(scene))
    {
    }

    CombatController::~CombatController()
    {
        SphereContactCollisionEvent::GetInstance()->RemoveListener(SphereContactCollisionEvent::GetInstanceId());
        MainPlayerActionEvent::GetInstance()->RemoveListener(MainPlayerActionEvent::GetInstanceId());
        PhysicsCollisionGameThreadEvent::GetInstance()->RemoveListener(PhysicsCollisionGameThreadEvent::GetInstanceId());
        RayCollisionEvent::GetInstance()->RemoveListener(RayCollisionEvent::GetInstanceId());
    }

    void CombatController::TempInitLevel()
    {
        const auto &sceneSp = mScene.lock();
        assert(sceneSp);
        FileFacade fileReader;
        fileReader.OpenAndReadFile("TestLevelName");
        const auto &lvlJsonStr = fileReader.GetFileSrc().front();

        LevelSerializationHelper lvlSerializationHelper;
        const auto lvlData = lvlSerializationHelper.RestoreLevelFromJsonString(lvlJsonStr);
        assert(lvlData.isDataValid());

        std::unordered_map<std::string, Path> pathRoutes;

        for (const auto &[routeName, route] : lvlData.RoutesData)
        {
            Path path;
            PathSegment segment;
            segment.SetSubdivisionsCount(50);
            for (const auto &routeControlPoint : route)
            {
                segment.SetControlPoints({{std::get<0>(routeControlPoint), std::get<1>(routeControlPoint), std::get<2>(routeControlPoint)}});
                path.AppendPathSegmentToTheEnd(segment);
            }
            pathRoutes.emplace(routeName, path);
        }
        mNavigationController->SetPathRoutes(pathRoutes);

        MaterialParser materialParser;
        for (const auto &[towerName, towerData] : lvlData.TowersData)
        {
            const auto &towerActor = mSpaceTowers.emplace_back(
                std::make_shared<Actor>(towerName, std::make_shared<SceneComponent>("c_root_" + towerName, std::get<0>(towerData), glm::vec3(), std::get<1>(towerData))));
            const auto &towerMaterialPrefab = materialParser.ParseMaterialDescriptor("PhysicalBasedMaterial.m");
            const std::string albedoName = "Space_Station_COLOR.png";
            const std::string normalName = "Space_Station_NORMAL.png";
            const std::string roughnessName = "Space_Station_ROUGHNESS.jpg";
            const std::string metallicName = "Space_Station_METALLIC.jpg";

            const auto &albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
            const auto &normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
            const auto &roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
            const auto &metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
            const float uvScale = 1.0f;
            MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "albedo", albedo_tex);
            MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "normalMap", normal_tex);
            MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "roughnessMap", roughness_tex);
            MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "metallicMap", metallic_tex);
            MaterialPropertySetter::SetMaterialPropertyValue(towerMaterialPrefab, "uvScale", uvScale);
            sceneSp->RegisterMaterialInstance(towerMaterialPrefab);

            const auto &d_mesh = std::make_shared<MeshComponentData>("c_mesh_" + towerName,
                                                                     "space_station.obj",
                                                                     glm::vec3(),
                                                                     glm::vec3(),
                                                                     glm::vec3(1),
                                                                     "",
                                                                     towerMaterialPrefab);
            const auto &meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
            const auto &c_mesh = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
            towerActor->AddComponent(c_mesh);
            sceneSp->AddActor(towerActor);
        }
    }

    void CombatController::OnPreLevelInit()
    {
        const auto thisSp = shared_from_this();
        SphereContactCollisionEvent::GetInstance()->AddListener(thisSp);
        MainPlayerActionEvent::GetInstance()->AddListener(thisSp);
        PhysicsCollisionGameThreadEvent::GetInstance()->AddListener(thisSp);
        RayCollisionEvent::GetInstance()->AddListener(thisSp);
        mNavigationController->OnPreLevelInit();
    }

    void CombatController::OnLevelInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            CreateWeaponBulletPool(sceneSp);

            WeakSpaceShipFactory spaceShipFactory;

            static constexpr int s_enemySpaceshipsCount = 5;
            for (size_t i = 0; i < s_enemySpaceshipsCount; ++i)
            {
                const auto &a_enemyShip = spaceShipFactory.CreateSpaceShip(sceneSp,
                                                                           glm::vec3(),
                                                                           glm::vec3(),
                                                                           glm::vec3(9));

                mEnemies.emplace_back(a_enemyShip);
            }

            CreateAsteroidsPool(sceneSp);
        }

        TempInitLevel();

        mNavigationController->SetEnemies(mEnemies);
        mNavigationController->OnLevelInit();
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

        for (const auto &asteroid : mSpaceObjectsPool)
        {
            asteroid->TriggerSpawn(GenRandomPositionForSpaceObject());
        }

        mNavigationController->PostPlayLevelFinished();
    }

    void CombatController::ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data)
    {
        const auto &playerAction = std::get<0>(data);

        if (eMainPlayerActionEnum::SHOOT == playerAction && !bIsCoolDownInProgress)
        {
            bIsCoolDownInProgress = true;
            if (const auto &sceneSp = mScene.lock())
            {
                ShootBullet(mPlayerShip->GetRootComponent()->GetTranslation());
                // todo: use player data provider instead
                LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, eMainPlayerStatusType::LIFE_POINTS_CHANGED);
            }
        }
        else if (eMainPlayerActionEnum::SELECT_NEXT_MISSILE_TYPE == playerAction || eMainPlayerActionEnum::SELECT_PREV_MISSILE_TYPE == playerAction)
        {
            static std::array<eMissileType, 4> missiles = {eMissileType::BOMB, eMissileType::FREEZING, eMissileType::ELECTRO_RAY, eMissileType::BLACK_HOLE};
            const auto &playerDataProvider = PlayerDataProvider::GetInstance();
            const auto selectedMissileType = playerDataProvider->GetSelectedMissileType();
            const auto foundMissileTypeIt = std::find(missiles.cbegin(), missiles.cend(), selectedMissileType);
            assert(foundMissileTypeIt != missiles.cend());
            const auto selectedMissileTypeIndex = std::distance(missiles.cbegin(), foundMissileTypeIt);
            const auto newMissileType = eMainPlayerActionEnum::SELECT_NEXT_MISSILE_TYPE == playerAction
                                            ? (missiles.size() - 1) == selectedMissileTypeIndex ? missiles.at(0) : missiles.at(selectedMissileTypeIndex + 1)
                                        : eMainPlayerActionEnum::SELECT_PREV_MISSILE_TYPE == playerAction
                                            ? 0 == selectedMissileTypeIndex ? missiles.at(missiles.size() - 1) : missiles.at(selectedMissileTypeIndex - 1)
                                            : eMissileType::NONE;
            assert(newMissileType != eMissileType::NONE);
            playerDataProvider->SetSelectedMissileType(newMissileType);
        }
    }

    void CombatController::ProcessEvent(const typename PhysicsCollisionGameThreadEvent::EventData_t &data)
    {
        const ePhysicsCollisionStateType collisionEventType = std::get<0>(data);
        const ePhysicsBodyType physBodyType = std::get<1>(data);
        const auto this_actor_id = std::get<3>(data);
        const auto that_actor_id = std::get<5>(data);

        if (ePhysicsBodyType::GHOST == physBodyType)
        {
            if (const auto &sceneSp = mScene.lock())
            {
                const std::string collisionType = ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType
                                                      ? "collision registered"
                                                      : "collision unregister";

                const auto &thisActorGameObjectType = GetGameObjectTypeByActorId(this_actor_id);
                const auto &thatActorGameObjectType = GetGameObjectTypeByActorId(that_actor_id);
                const auto &objectsCollisionType = GetGameObjectsCollisionType(thisActorGameObjectType, thatActorGameObjectType);

                if (objectsCollisionType != eGameObjectsCollisionType::UNDEFINED)
                {
                    if (eGameObjectsCollisionType::SPACESHIP_WITH_MISSILE == objectsCollisionType)
                    {
                        const auto &ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType ? GetEnemyShipOwnerActorById(this_actor_id) : GetEnemyShipOwnerActorById(that_actor_id);
                        const auto &ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType ? GetMissileOwnerActorById(this_actor_id) : GetMissileOwnerActorById(that_actor_id);
                        const auto &spaceshipActor_id = eGameObjectsType::SPACESHIP == thisActorGameObjectType ? this_actor_id : that_actor_id;
                        const auto &missileActor_id = eGameObjectsType::MISSILE == thisActorGameObjectType ? this_actor_id : that_actor_id;

                        const auto &concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
                                                               ? ownerMissileActor
                                                               : ownerMissileActor->GetChildByObjectId(missileActor_id);

                        const auto &concreteSpaceshipActor = ownerEnemyShipActor->GetObjectId() == spaceshipActor_id
                                                                 ? ownerEnemyShipActor
                                                                 : ownerEnemyShipActor->GetChildByObjectId(spaceshipActor_id);

                        LogInfo("CombatController::PhysicsCollisionGameThreadEvent =>", collisionType, "missile with spaceship, this_actor = ", concreteMissileActor->GetName(), " that_actor = ",
                                concreteSpaceshipActor->GetName());

                        const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
                        if (ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType)
                        {
                            explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor);
                        }
                        else
                        {
                            explosionVisitor->EndExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor);
                        }
                    }
                    else if (eGameObjectsCollisionType::SPACESHIP_WITH_NEUTRAL_SPACE_OBJECT == objectsCollisionType)
                    {
                        const auto &ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType ? GetEnemyShipOwnerActorById(this_actor_id) : GetEnemyShipOwnerActorById(that_actor_id);
                        const auto &ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType ? GetSpaceObjectOwnerActorById(this_actor_id) : GetSpaceObjectOwnerActorById(that_actor_id);

                        LogInfo("CombatController::PhysicsCollisionGameThreadEvent =>", collisionType, "spaceship with space object, this_actor = ", ownerEnemyShipActor->GetName(),
                                " that_actor = ", ownerSpaceObjectActor->GetName());

                        ownerEnemyShipActor->TriggerDamageReceived(1UL, eDamageDealerType::NEUTRAL_OBJECT);
                        ownerSpaceObjectActor->TriggerDisabled();
                    }
                    else if (eGameObjectsCollisionType::MISSILE_WITH_NEUTRAL_SPACE_OBJECT == objectsCollisionType)
                    {
                        const auto &ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType ? GetMissileOwnerActorById(this_actor_id) : GetMissileOwnerActorById(that_actor_id);
                        const auto &ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType ? GetSpaceObjectOwnerActorById(this_actor_id) : GetSpaceObjectOwnerActorById(that_actor_id);
                        const auto &missileActor_id = eGameObjectsType::MISSILE == thisActorGameObjectType ? this_actor_id : that_actor_id;

                        const auto &concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
                                                               ? ownerMissileActor
                                                               : ownerMissileActor->GetChildByObjectId(missileActor_id);

                        LogInfo("CombatController::PhysicsCollisionGameThreadEvent => ", collisionType, "missile with space object, this_actor = ", concreteMissileActor->GetName(),
                                " that_actor = ", ownerSpaceObjectActor->GetName());

                        const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
                        if (ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType)
                        {
                            explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor);
                        }
                        else
                        {
                            explosionVisitor->EndExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor);
                        }
                    }
                }
            }
        }
    }

    void CombatController::ProcessEvent(const typename RayCollisionEvent::EventData_t &data)
    {
        const auto &eventSenderMissileWp = std::get<0>(data);
        const auto &collidedActorWp = std::get<1>(data);

        if (const auto &rayMissileActorSp = eventSenderMissileWp.lock())
        {
            if (const auto &collidedActorSp = collidedActorWp.lock())
            {
                const auto &gameObjectType = GetGameObjectTypeByActorId(collidedActorSp->GetObjectId());
                if (eGameObjectsType::UNDEFINED != gameObjectType)
                {
                    const auto explosionVisitor = rayMissileActorSp->CreateMissileExplosionVisitor();
                    if (eGameObjectsType::SPACESHIP == gameObjectType)
                    {
                        const auto &ownerEnemyShipActor = GetEnemyShipOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, rayMissileActorSp);
                    }
                    else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType)
                    {
                        const auto &ownerSpaceObjectActor = GetSpaceObjectOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, rayMissileActorSp);
                    }
                }
            }
        }
    }

    void CombatController::ProcessEvent(const typename SphereContactCollisionEvent::EventData_t &data)
    {
        const auto &srcActorId = std::get<0>(data);
        const auto &collidedActorIds = std::move(std::get<1>(data));

        const auto &srcActorGameObjectType = GetGameObjectTypeByActorId(srcActorId);
        if (eGameObjectsType::UNDEFINED != srcActorGameObjectType && eGameObjectsType::MISSILE != srcActorGameObjectType)
        {
            const std::shared_ptr<Actor> &srcCollisionActor = eGameObjectsType::SPACESHIP == srcActorGameObjectType
                                                                  ? std::static_pointer_cast<Actor>(GetEnemyShipOwnerActorById(srcActorId))
                                                              : eGameObjectsType::NEUTRAL_SPACE_OBJECT == srcActorGameObjectType
                                                                  ? std::static_pointer_cast<Actor>(GetSpaceObjectOwnerActorById(srcActorId))
                                                                  : nullptr;
            assert(srcCollisionActor);
            for (const auto &collidedActorId : collidedActorIds)
            {
                const auto &gameObjectType = GetGameObjectTypeByActorId(collidedActorId);
                if (eGameObjectsType::UNDEFINED != gameObjectType)
                {
                    if (eGameObjectsType::SPACESHIP == gameObjectType)
                    {
                        const auto &ownerEnemyShipActor = GetEnemyShipOwnerActorById(collidedActorId);
                        const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(std::make_pair(gameObjectType, ownerEnemyShipActor),
                                                                                                       std::make_pair(srcActorGameObjectType, srcCollisionActor));
                        electroRayChainModifier->Initialize(mElectroRayChainActorPool);
                        ownerEnemyShipActor->AddModifier(electroRayChainModifier);
                    }
                    else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType)
                    {
                        const auto &ownerSpaceObjectActor = GetSpaceObjectOwnerActorById(collidedActorId);
                        const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(std::make_pair(gameObjectType, ownerSpaceObjectActor),
                                                                                                       std::make_pair(srcActorGameObjectType, srcCollisionActor));

                        electroRayChainModifier->Initialize(mElectroRayChainActorPool);
                        ownerSpaceObjectActor->AddModifier(electroRayChainModifier);
                    }
                }
            }
        }
    }

    void CombatController::Tick(const float deltaTime)
    {
        FlushToPoolUsedBullets();
        UpdateMissilesData();

        if (bIsCoolDownInProgress)
        {
            mDeltaTime += deltaTime;

            if (mDeltaTime > mCoolDownTime)
            {
                mDeltaTime = 0.0f;
                bIsCoolDownInProgress = false;
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

        mNavigationController->Tick(deltaTime);
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
        static constexpr auto freezingMissileCount = 1, bombMissileCount = 1, blackHoleMissileCount = 1, electroRayCount = 1;

        ElectroRayFactory electroRayFactory;
        for (size_t i = 0; i < electroRayCount; ++i)
        {
            const auto &a_electroRay = electroRayFactory.CreateMissile(sceneSp,
                                                                       mPlayerShip,
                                                                       glm::vec3(0),
                                                                       glm::vec3(),
                                                                       glm::vec3(1.0));

            mMissilesPool.emplace_back(a_electroRay);
        }

        FreezingMissileFactory freezingMissileFactory;
        for (size_t i = 0; i < freezingMissileCount; ++i)
        {
            const auto &a_missile = freezingMissileFactory.CreateMissile(sceneSp,
                                                                         mPlayerShip,
                                                                         glm::vec3(0),
                                                                         glm::vec3(),
                                                                         glm::vec3(1.0));

            mMissilesPool.emplace_back(a_missile);
        }

        BombMissileFactory bombMissileFactory;
        for (size_t i = 0; i < bombMissileCount; ++i)
        {
            const auto &a_missile = bombMissileFactory.CreateMissile(sceneSp,
                                                                     mPlayerShip,
                                                                     glm::vec3(0),
                                                                     glm::vec3(),
                                                                     glm::vec3(1.0));

            mMissilesPool.emplace_back(a_missile);
        }

        BlackHoleMissileFactory blackHoleMissileFactory;
        for (size_t i = 0; i < blackHoleMissileCount; ++i)
        {
            const auto &a_missile = blackHoleMissileFactory.CreateMissile(sceneSp,
                                                                          mPlayerShip,
                                                                          glm::vec3(0),
                                                                          glm::vec3(),
                                                                          glm::vec3(1.0));

            mMissilesPool.emplace_back(a_missile);
        }

        std::unordered_map<eMissileType, size_t> availabeMissileTypes;
        if (bombMissileCount)
        {
            availabeMissileTypes.emplace(eMissileType::BOMB, bombMissileCount);
        }
        if (freezingMissileCount)
        {
            availabeMissileTypes.emplace(eMissileType::FREEZING, freezingMissileCount);
        }
        if (electroRayCount)
        {
            availabeMissileTypes.emplace(eMissileType::ELECTRO_RAY, electroRayCount);
        }
        if (blackHoleMissileCount)
        {
            availabeMissileTypes.emplace(eMissileType::BLACK_HOLE, blackHoleMissileCount);
        }
        PlayerDataProvider::GetInstance()->SetAvailableMissileTypes(availabeMissileTypes);
    }

    void CombatController::CreateAsteroidsPool(const std::shared_ptr<Scene> &sceneSp)
    {
        AsteroidFactory asteroidFactory;
        static constexpr int s_asteroidsCount = 0;
        for (size_t i = 0; i < s_asteroidsCount; ++i)
        {
            const auto &a_asteroid = asteroidFactory.CreateSpaceObject(sceneSp, glm::vec3(0),
                                                                       glm::vec3(),
                                                                       glm::vec3(40.0));
            mSpaceObjectsPool.emplace_back(a_asteroid);
        }
    }

    void CombatController::ShootBullet(const glm::vec3 &bulletStartPosition)
    {
        const auto selectedMissileType = PlayerDataProvider::GetInstance()->GetSelectedMissileType();
        auto idleBulletIt = std::find_if(mMissilesPool.begin(), mMissilesPool.end(), [selectedMissileType](const auto &missile)
                                         { return (eMissileActivityState::IDLE == missile->GetMissileActivityState() && selectedMissileType == missile->GetMissileType()); });

        if (idleBulletIt == mMissilesPool.end())
        {
            LogInfo("CombatController::ShootBullet => Error - no idle bullets in the pool");
            return;
        }

        (*idleBulletIt)->TriggerSpawn(bulletStartPosition, eDamageDealerType::MAIN_PLAYER);
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

    void CombatController::UpdateMissilesData()
    {
        std::unordered_map<eMissileType, size_t> missiles;
        const auto &availableMissileTypes = PlayerDataProvider::GetInstance()->GetAvailableMissileTypes();
        for (const auto &avlMissileType : availableMissileTypes)
        {
            missiles[avlMissileType] = 0;
        }
        for (auto &missile : mMissilesPool)
        {
            if (eMissileActivityState::IDLE == missile->GetMissileActivityState())
            {
                missiles[missile->GetMissileType()] = missiles.at(missile->GetMissileType()) + 1;
            }
        }
        PlayerDataProvider::GetInstance()->SetMissilesCount(missiles);
    }

    std::shared_ptr<MissileActor> CombatController::GetMissileOwnerActorById(const int32_t actorId) const
    {
        const auto foundIt = std::find_if(mMissilesPool.cbegin(),
                                          mMissilesPool.cend(),
                                          [=](const auto &missile)
                                          {
                                              return missile->HasEngineObjectIdInHierarchy(actorId);
                                          });
        return (*foundIt);
    }

    std::shared_ptr<SpaceshipActor> CombatController::GetEnemyShipOwnerActorById(const int32_t actorId) const
    {
        const auto foundIt = std::find_if(mEnemies.cbegin(), mEnemies.cend(), [actorId](const auto &enemyActor)
                                          { return enemyActor->HasEngineObjectIdInHierarchy(actorId); });
        return (*foundIt);
    }

    std::shared_ptr<SpaceObjectActor> CombatController::GetSpaceObjectOwnerActorById(const int32_t actorId) const
    {
        const auto foundIt = std::find_if(mSpaceObjectsPool.cbegin(),
                                          mSpaceObjectsPool.cend(),
                                          [=](const auto &spaceObject)
                                          {
                                              return spaceObject->HasEngineObjectIdInHierarchy(actorId);
                                          });
        return (*foundIt);
    }

    eGameObjectsType CombatController::GetGameObjectTypeByActorId(const int32_t actorId) const
    {
        eGameObjectsType result{eGameObjectsType::UNDEFINED};

        result = std::any_of(mEnemies.begin(), mEnemies.end(), [actorId](const auto &spaceshipActor)
                             { return spaceshipActor->HasEngineObjectIdInHierarchy(actorId); })
                     ? eGameObjectsType::SPACESHIP
                     : eGameObjectsType::UNDEFINED;

        if (eGameObjectsType::UNDEFINED == result)
        {
            result = std::any_of(mMissilesPool.begin(), mMissilesPool.end(), [actorId](const auto &missileActor)
                                 { return missileActor->HasEngineObjectIdInHierarchy(actorId); })
                         ? eGameObjectsType::MISSILE
                         : eGameObjectsType::UNDEFINED;
        }

        if (eGameObjectsType::UNDEFINED == result)
        {
            result = std::any_of(mSpaceObjectsPool.begin(), mSpaceObjectsPool.end(), [actorId](const auto &spaceObjectActor)
                                 { return spaceObjectActor->HasEngineObjectIdInHierarchy(actorId); })
                         ? eGameObjectsType::NEUTRAL_SPACE_OBJECT
                         : eGameObjectsType::UNDEFINED;
        }

        assert(eGameObjectsType::UNDEFINED != result);

        return result;
    }

    eGameObjectsCollisionType CombatController::GetGameObjectsCollisionType(const eGameObjectsType firstObject, const eGameObjectsType secondObject) const
    {
        if ((eGameObjectsType::SPACESHIP == firstObject && eGameObjectsType::MISSILE == secondObject) ||
            (eGameObjectsType::MISSILE == firstObject && eGameObjectsType::SPACESHIP == secondObject))
            return eGameObjectsCollisionType::SPACESHIP_WITH_MISSILE;

        if ((eGameObjectsType::SPACESHIP == firstObject && eGameObjectsType::NEUTRAL_SPACE_OBJECT == secondObject) ||
            (eGameObjectsType::NEUTRAL_SPACE_OBJECT == firstObject && eGameObjectsType::SPACESHIP == secondObject))
            return eGameObjectsCollisionType::SPACESHIP_WITH_NEUTRAL_SPACE_OBJECT;

        if ((eGameObjectsType::MISSILE == firstObject && eGameObjectsType::NEUTRAL_SPACE_OBJECT == secondObject) ||
            (eGameObjectsType::NEUTRAL_SPACE_OBJECT == firstObject && eGameObjectsType::MISSILE == secondObject))
            return eGameObjectsCollisionType::MISSILE_WITH_NEUTRAL_SPACE_OBJECT;

        return eGameObjectsCollisionType::UNDEFINED;
    }

    void CombatController::CleanUp()
    {
    }
}