#include "CombatController.h"
#include "Core/GameCore/Scene.h"

#include "Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h"
#include "Implementation/Modifiers/ElectroRayChainModifier.h"
#include "Implementation/GalaxySceneCamera.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/Levels/LevelSerializationHelper.h"
#include "Implementation/Navigation/PathSegment.h"
#include "Implementation/Navigation/Path.h"
#include "Implementation/Factories/SpaceStationFactory.h"
#include "Implementation/Actors/BlackHoleMissileActor.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/SphereCollisionTestWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/CommonCore/Random.h"

#include <array>
#include <unordered_map>
#include <tuple>
#include <json/json.hpp>

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;

#undef min // without it conflicts with math min, max function
#undef max

namespace Game
{

    CombatController::CombatController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mLevelBounds(BoundingBox3D(glm::vec3(0), glm::vec3(100, 50, 100))),
          mNavigationController(std::make_shared<NavigationController>(scene)),
          mUserInteractionController(std::make_shared<UserInteractionController>(scene)),
          mCombatActorsPoolHandler(std::make_shared<CombatActorsPoolHandler>(scene))
    {
    }

    CombatController::~CombatController()
    {
        ElectroRaySphereContactCollisionEvent::GetInstance()->RemoveListener(ElectroRaySphereContactCollisionEvent::GetInstanceId());
        PhysicsCollisionGameThreadEvent::GetInstance()->RemoveListener(PhysicsCollisionGameThreadEvent::GetInstanceId());
        ElectroRayCollisionEvent::GetInstance()->RemoveListener(ElectroRayCollisionEvent::GetInstanceId());
        BroadcastGameThreadEvent::GetInstance()->RemoveListener(BroadcastGameThreadEvent::GetInstanceId());
        ChangeGameModeEvent::GetInstance()->RemoveListener(ChangeGameModeEvent::GetInstance()->GetInstanceId());
    }

    void CombatController::OnPreLevelInit()
    {
        const auto thisSp = shared_from_this();
        ElectroRaySphereContactCollisionEvent::GetInstance()->AddListener(thisSp);
        PhysicsCollisionGameThreadEvent::GetInstance()->AddListener(thisSp);
        ElectroRayCollisionEvent::GetInstance()->AddListener(thisSp);
        BroadcastGameThreadEvent::GetInstance()->AddListener(thisSp);
        ChangeGameModeEvent::GetInstance()->AddListener(thisSp);
        mNavigationController->OnPreLevelInit();
        mUserInteractionController->OnPreLevelInit();
    }

    void CombatController::InitFromLevelData(const LevelData &levelData)
    {
        assert(levelData.isDataValid());
        const auto &sceneSp = mScene.lock();
        assert(sceneSp);

        mLevelBounds = BoundingBox3D(glm::vec3(0.0f),
                                     glm::vec3(std::abs(levelData.LevelBoundaryMax.x - levelData.LevelBoundaryMin.x) * 0.5f,
                                               50.0f,
                                               std::abs(levelData.LevelBoundaryMax.y - levelData.LevelBoundaryMin.y) * 0.5));
        mNavigationController->SetLevelBounds(mLevelBounds);

        std::unordered_map<std::string, Path> pathRoutes;

        for (const auto &[routeName, route] : levelData.RoutesData)
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

        for (const auto &[towerName, towerData] : levelData.TowersData)
        {
            const glm::vec3 &position = std::get<0>(towerData);
            const glm::vec3 &scale = std::get<1>(towerData);
            mCombatActorsPoolHandler->CreateSpaceStationActor(towerName, position, glm::vec3(), scale);
        }

        // todo: when create a new portal first check if another portals could be on the same point
        // If more than one portal is on one start point - remove duplicated portals
        mCombatActorsPoolHandler->SpawnPortals(pathRoutes.size(), 10.0f);
        for (const auto &[pathName, pathData] : pathRoutes)
        {
            const auto &portalSp = mCombatActorsPoolHandler->GetFreePortalActor();
            assert(portalSp);
            portalSp->SetIsEnabled(true);
            pathData.GetRoutePoints();
            portalSp->GetRootComponent()->SetTranslation(pathData.GetRouteFirstPoint());
        }

        for (const auto &[barrierName, barrierData] : levelData.BarriersData)
        {
            mCombatActorsPoolHandler->SpawnBarriers(1, barrierData.size());
            const auto &a_barrier = mCombatActorsPoolHandler->GetFreeBarrierActor();
            assert(a_barrier && barrierData.size() == a_barrier->GetBarrierPillarsCount());
            int32_t pillarIndex = 0;
            for (const auto &pillarPosition : barrierData)
            {
                a_barrier->TrySetBarrierPillarMeshRelativeTransform(pillarIndex++, pillarPosition, glm::vec3(), glm::vec3(6.0f, 12.0f, 6.0f));
            }
            a_barrier->SetIsEnabled(true);
        }
    }

    void CombatController::OnLevelInit()
    {
        const int32_t c_bombMissilesCount = 10 * mCombatActorsPoolHandler->GetSpaceStationsCount();
        mCombatActorsPoolHandler->SpawnEnemySpaceships(10);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::BOMB, c_bombMissilesCount);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::FREEZING, 3);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::ELECTRO_RAY, 1);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::BLACK_HOLE, 2);

        const std::unordered_map<eMissileType, size_t> availabeMissileTypes = {
            {eMissileType::BOMB, 10},
            {eMissileType::FREEZING, 3},
            {eMissileType::ELECTRO_RAY, 1},
            {eMissileType::BLACK_HOLE, 2}};
        PlayerDataProvider::GetInstance()->SetAvailableMissileTypes(availabeMissileTypes);

        mCombatActorsPoolHandler->SpawnAsteroids(20);
        mCombatActorsPoolHandler->SpawnBarriers(1, 5);
        const auto &lvlBoundaryMin = mLevelBounds.GetMin();
        const auto &lvlBoundaryMax = mLevelBounds.GetMax();
        const auto &lvlBoundaryOrigin = mLevelBounds.GetOrigin();
        if (const auto &a_barrierSp = mCombatActorsPoolHandler->GetFreeBarrierActor())
        {
            a_barrierSp->SetIsEnabled(true);
            a_barrierSp->GetRootComponent()->SetTranslation(glm::vec3(0, lvlBoundaryOrigin.y, 0));
            a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(0, glm::vec3(lvlBoundaryMin.x, 0, lvlBoundaryMin.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
            a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(1, glm::vec3(lvlBoundaryMin.x, 0, lvlBoundaryMax.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
            a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(2, glm::vec3(lvlBoundaryMax.x, 0, lvlBoundaryMax.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
            a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(3, glm::vec3(lvlBoundaryMax.x, 0, lvlBoundaryMin.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
            a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(4, glm::vec3(lvlBoundaryMin.x, 0, lvlBoundaryMin.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
        }
        mNavigationController->OnLevelInit();
        mUserInteractionController->SetActorsPoolHandler(mCombatActorsPoolHandler);
        mUserInteractionController->SetOnShootCallback(std::bind(&CombatController::OnReadyToShoot, this));
        mUserInteractionController->OnLevelInit();
    }

    void CombatController::OnPostLevelInit()
    {
        mNavigationController->OnPostLevelInit();
        mUserInteractionController->OnPostLevelInit();
    }

    void CombatController::PostPlayLevelFinished()
    {
        mNavigationController->PostPlayLevelFinished();
        mUserInteractionController->PostPlayLevelFinished();

        ChangeGameModeEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eGameModeType::SPACE_STATION_PLACEMENT);
    }

    void CombatController::OnCombatPreparationCompleted()
    {
        const auto &pathNames = mNavigationController->GetPathNames();
        for (const auto &pathName : pathNames)
        {

            mSpawnEnemyOnRouteTimers.try_emplace(pathName);
            auto &timer = mSpawnEnemyOnRouteTimers[pathName];
            timer.SetIntervalMs(1500);
            timer.SetIsPausable(true);
            timer.SetIsRepeat(true);
            timer.SetCallback([pathName, this]()
                              { 
                                if (const auto &freeShip = mCombatActorsPoolHandler->GetFreeSpaceshipActor())
                                {
                                    mNavigationController->PutSpaceshipOnRoute(pathName, freeShip); 
                                } });

            timer.StartTimer();
        }
    }

    void CombatController::OnReadyToShoot()
    {
        LogInfo("CombatController::OnReadyToShoot");
        const auto &projectileMarkerPosition = mUserInteractionController->GetProjectileMarkerPosition();
        const auto selectedSpaceStationId = mUserInteractionController->GetSelectedSpaceStationId();
        const auto &activeSpaceStationActor = mCombatActorsPoolHandler->GetSpaceStationOwnerActorById(selectedSpaceStationId);
        assert(activeSpaceStationActor);
        const auto &activeSpaceStationPosition = activeSpaceStationActor->GetRootComponent()->GetTranslation();
        const auto &projectileShootDirection = glm::normalize(projectileMarkerPosition - activeSpaceStationPosition);
        const auto selectedMissileType = PlayerDataProvider::GetInstance()->GetSelectedMissileType();
        LaunchMisile(activeSpaceStationActor, activeSpaceStationPosition, projectileShootDirection, selectedMissileType);
    }

    void CombatController::ProcessEvent(const PhysicsCollisionGameThreadEvent *sender, const typename PhysicsCollisionGameThreadEvent::EventData_t &data)
    {
        const ePhysicsCollisionStateType collisionEventType = std::get<0>(data);
        const ePhysicsBodyType physBodyType = std::get<1>(data);
        const auto this_actor_id = std::get<3>(data);
        const auto that_actor_id = std::get<5>(data);

        const auto &sceneSp = mScene.lock();
        if (!sceneSp || ePhysicsBodyType::GHOST != physBodyType)
            return;

        const std::string collisionType = ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType
                                              ? "collision registered"
                                              : "collision unregister";

        const auto thisActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(this_actor_id);
        const auto thatActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(that_actor_id);
        const auto objectsCollisionType = mCombatActorsPoolHandler->GetGameObjectsCollisionType(thisActorGameObjectType, thatActorGameObjectType);

        if (objectsCollisionType == eGameObjectsCollisionType::UNDEFINED)
            return;
        if (eGameObjectsCollisionType::SPACESHIP_WITH_MISSILE == objectsCollisionType)
        {
            const auto &ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType
                                                  ? mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(this_actor_id)
                                                  : mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(that_actor_id);

            const auto &ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType
                                                ? mCombatActorsPoolHandler->GetMissileOwnerActorById(this_actor_id)
                                                : mCombatActorsPoolHandler->GetMissileOwnerActorById(that_actor_id);

            const auto spaceshipActor_id = eGameObjectsType::SPACESHIP == thisActorGameObjectType ? this_actor_id : that_actor_id;
            const auto missileActor_id = eGameObjectsType::MISSILE == thisActorGameObjectType ? this_actor_id : that_actor_id;

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
            const auto &ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType
                                                  ? mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(this_actor_id)
                                                  : mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(that_actor_id);
            const auto &ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType
                                                    ? mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(this_actor_id)
                                                    : mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(that_actor_id);

            LogInfo("CombatController::PhysicsCollisionGameThreadEvent =>", collisionType, "spaceship with space object, this_actor = ", ownerEnemyShipActor->GetName(),
                    " that_actor = ", ownerSpaceObjectActor->GetName());

            ownerSpaceObjectActor->TriggerDisabled();
            ownerEnemyShipActor->TriggerDamageReceived(1UL, eDamageDealerType::NEUTRAL_OBJECT);
        }
        else if (eGameObjectsCollisionType::MISSILE_WITH_NEUTRAL_SPACE_OBJECT == objectsCollisionType)
        {
            const auto &ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType
                                                ? mCombatActorsPoolHandler->GetMissileOwnerActorById(this_actor_id)
                                                : mCombatActorsPoolHandler->GetMissileOwnerActorById(that_actor_id);

            const auto &ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType
                                                    ? mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(this_actor_id)
                                                    : mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(that_actor_id);
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

    void CombatController::ProcessEvent(const ElectroRayCollisionEvent *sender, const typename ElectroRayCollisionEvent::EventData_t &data)
    {
        const auto &eventSenderMissileWp = std::get<0>(data);
        const auto &collidedActorWp = std::get<1>(data);

        if (const auto &rayMissileActorSp = eventSenderMissileWp.lock())
        {
            if (const auto &collidedActorSp = collidedActorWp.lock())
            {
                const auto &gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedActorSp->GetObjectId());
                if (eGameObjectsType::UNDEFINED != gameObjectType)
                {
                    const auto explosionVisitor = rayMissileActorSp->CreateMissileExplosionVisitor();
                    if (eGameObjectsType::SPACESHIP == gameObjectType)
                    {
                        const auto &ownerEnemyShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, rayMissileActorSp);
                    }
                    else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType)
                    {
                        const auto &ownerSpaceObjectActor = mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, rayMissileActorSp);
                    }
                }
            }
        }
    }

    void CombatController::ProcessEvent(const ElectroRaySphereContactCollisionEvent *sender, const typename ElectroRaySphereContactCollisionEvent::EventData_t &data)
    {
        const auto &srcActorId = std::get<0>(data);
        const auto &collidedActorIds = std::move(std::get<1>(data));

        const auto &srcActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(srcActorId);
        if (eGameObjectsType::UNDEFINED != srcActorGameObjectType && eGameObjectsType::MISSILE != srcActorGameObjectType)
        {
            const std::shared_ptr<Actor> &srcCollisionActor = eGameObjectsType::SPACESHIP == srcActorGameObjectType
                                                                  ? std::static_pointer_cast<Actor>(mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(srcActorId))
                                                              : eGameObjectsType::NEUTRAL_SPACE_OBJECT == srcActorGameObjectType
                                                                  ? std::static_pointer_cast<Actor>(mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(srcActorId))
                                                                  : nullptr;
            assert(srcCollisionActor);
            for (const auto &collidedActorId : collidedActorIds)
            {
                const auto &gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedActorId);
                if (eGameObjectsType::UNDEFINED != gameObjectType)
                {
                    if (eGameObjectsType::SPACESHIP == gameObjectType)
                    {
                        const auto &ownerEnemyShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(collidedActorId);
                        const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(std::make_pair(gameObjectType, ownerEnemyShipActor),
                                                                                                       std::make_pair(srcActorGameObjectType, srcCollisionActor));
                        electroRayChainModifier->Initialize(mCombatActorsPoolHandler->GetFreeElectroChainActor());
                        ownerEnemyShipActor->AddModifier(electroRayChainModifier);
                    }
                    else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType)
                    {
                        const auto &ownerSpaceObjectActor = mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(collidedActorId);
                        const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(std::make_pair(gameObjectType, ownerSpaceObjectActor),
                                                                                                       std::make_pair(srcActorGameObjectType, srcCollisionActor));

                        electroRayChainModifier->Initialize(mCombatActorsPoolHandler->GetFreeElectroChainActor());
                        ownerSpaceObjectActor->AddModifier(electroRayChainModifier);
                    }
                }
            }
        }
    }

    void CombatController::ProcessEvent(const BroadcastGameThreadEvent *sender, const typename BroadcastGameThreadEvent::EventData_t &data)
    {
        const auto &eventHeader = std::get<0>(data);
        const auto &jsonParams = std::get<1>(data);
        if ("CombatLevelEvents" == eventHeader)
        {
            const auto &jsonObj = nlohmann::json::parse(std::get<1>(data));
            const auto &actionName = jsonObj.at("action").get<std::string>();
            if ("button_press" == actionName)
            {
                const auto selectedSpaceStationId = mUserInteractionController->GetSelectedSpaceStationId();
                if (selectedSpaceStationId >= 0)
                {
                    const auto &buttonType = jsonObj.at("button_type").get<std::string>();
                    static std::unordered_map<std::string, eMissileType> missilesMap = {{"Bomb", eMissileType::BOMB},
                                                                                        {"Freezing", eMissileType::FREEZING},
                                                                                        {"Electro_Ray", eMissileType::ELECTRO_RAY},
                                                                                        {"Black_Hole", eMissileType::BLACK_HOLE}};
                    ext_assert(missilesMap.count(buttonType), "Unknown button type: " + buttonType);
                    PlayerDataProvider::GetInstance()->SetSelectedMissileType(missilesMap.at(buttonType));
                    mUserInteractionController->ShowMissileProjectile();
                }
            }
        }
    }

    void CombatController::ProcessEvent(const ChangeGameModeEvent *sender, const typename ChangeGameModeEvent::EventData_t &data)
    {
        const auto newValue = std::get<0>(data);
        if (newValue != mGameModeType)
        {
            if (eGameModeType::SPACE_STATION_PLACEMENT == mGameModeType &&
                eGameModeType::COMBAT == newValue)
            {
                OnCombatPreparationCompleted();
            }

            if (eGameModeType::INIT == mGameModeType &&
                eGameModeType::SPACE_STATION_PLACEMENT == newValue)
            {
                if (const auto &sceneSp = mScene.lock())
                {
                    static constexpr auto functionId = Hash64_CT("CombatController::LuaChangeGameModeEvent");
                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, 0, functionId, []()
                                                                                     { LuaChangeGameModeEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                                                                                                        eGameModeType::SPACE_STATION_PLACEMENT); });
                }
            }

            mGameModeType = newValue;
        }
    }

    void CombatController::Tick(const float deltaTime)
    {
        if (eGameModeType::COMBAT == mGameModeType)
        {
            ValidatePoolObjects();
            UpdateMissilesData();
            ProcessAiAction();
            mNavigationController->Tick(deltaTime);
        }

        mUserInteractionController->Tick(deltaTime);
    }

    void CombatController::LaunchMisile(const std::shared_ptr<Actor> &missileOwner,
                                        const glm::vec3 &missileStartPosition,
                                        const glm::vec3 &missileDirection,
                                        const eMissileType missileType)
    {
        const auto &missile = mCombatActorsPoolHandler->GetFreeMissile(missileType);
        if (missile)
        {
            const auto yawRad = std::atan2(missileDirection.x, missileDirection.z);
            const auto yawDeg = RAD_TO_DEG(yawRad);
            missile->TriggerSpawn(missileStartPosition, missileDirection, yawDeg, eDamageDealerType::MAIN_PLAYER, missileOwner);
            mNavigationController->PutMissileToNavigate(missile);
        }
        else
        {
            LogInfo("CombatController::LaunchMisile => No free missiles!");
        }
    }

    void CombatController::ValidatePoolObjects()
    {
        const auto &enemySpaceshipActors = mCombatActorsPoolHandler->GetEnemySpaceshipActors();

        if (enemySpaceshipActors.size())
        {
            int32_t returnedToPoolSpaceships = 0;
            for (const auto &enemySpaceship : enemySpaceshipActors)
            {
                if (eSpaceshipActivityState::PENDING_DISABLE == enemySpaceship->GetSpaceshipActivityState())
                {
                    mNavigationController->RemoveSpaceshipFromRoute(enemySpaceship->GetObjectId());
                    enemySpaceship->TriggerDisabled();
                    ++returnedToPoolSpaceships;
                }
            }
            if (returnedToPoolSpaceships)
            {
                LogInfo("CombatController::ValidatePoolObjects => returnedToPoolSpaceships: ", returnedToPoolSpaceships);
            }
        }

        const auto &missileActors = mCombatActorsPoolHandler->GetMissileActors();

        if (missileActors.size())
        {
            int32_t returnedToPoolMissiles = 0;
            for (const auto &missileActor : missileActors)
            {
                if (eMissileActivityState::OUT_OF_LEVEL == missileActor->GetMissileActivityState())
                {
                    mNavigationController->RemoveMissileFromNavigation(missileActor->GetObjectId());
                    missileActor->TriggerDisabled();
                    ++returnedToPoolMissiles;
                }
            }
            if (returnedToPoolMissiles)
            {
                LogInfo("CombatController::ValidatePoolObjects => returnedToPoolMissiles: ", returnedToPoolMissiles);
            }
        }
    }

    void CombatController::UpdateMissilesData()
    {
        // std::unordered_map<eMissileType, size_t> missiles;
        // const auto &availableMissileTypes = PlayerDataProvider::GetInstance()->GetAvailableMissileTypes();
        // for (const auto &avlMissileType : availableMissileTypes)
        // {
        //     missiles[avlMissileType] = 0;
        // }
        // for (auto &missile : mMissilesPool)
        // {
        //     if (eMissileActivityState::IDLE == missile->GetMissileActivityState())
        //     {
        //         missiles[missile->GetMissileType()] = missiles.at(missile->GetMissileType()) + 1;
        //     }
        // }
        // PlayerDataProvider::GetInstance()->SetMissilesCount(missiles);
    }

    void CombatController::ProcessAiAction()
    {
        const auto &sceneSp = mScene.lock();
        if (!sceneSp)
        {
            return;
        }

        const auto &spaceStations = mCombatActorsPoolHandler->GetSpaceStationActors();
        std::vector<std::shared_ptr<PhysicsComponent>> excludedPhysicsComponents;
        const auto &spaceStationsPhysComponents = mCombatActorsPoolHandler->GetSpaceStationsPhysicsComponents();
        const auto &bombMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BOMB);
        const auto &freezeMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::FREEZING);
        const auto &blackHoleMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BLACK_HOLE);
        excludedPhysicsComponents.reserve(spaceStations.size() + mCombatActorsPoolHandler->GetMissileActors().size());
        excludedPhysicsComponents.insert(excludedPhysicsComponents.end(), spaceStationsPhysComponents.begin(), spaceStationsPhysComponents.end());
        excludedPhysicsComponents.insert(excludedPhysicsComponents.end(), bombMissilePhysComponents.begin(), bombMissilePhysComponents.end());
        excludedPhysicsComponents.insert(excludedPhysicsComponents.end(), freezeMissilePhysComponents.begin(), freezeMissilePhysComponents.end());
        excludedPhysicsComponents.insert(excludedPhysicsComponents.end(), blackHoleMissilePhysComponents.begin(), blackHoleMissilePhysComponents.end());

        for (const auto &spaceStation : spaceStations)
        {
            if (spaceStation->CanShoot())
            {
                constexpr float c_collisionSphereRadius = 50.0f;
                SphereCollisionTestWithFilterAdapter collisionTest(c_collisionSphereRadius, excludedPhysicsComponents);
                collisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), spaceStation->GetRootComponent()->GetTranslation());
                const auto &collidedDescriptors = collisionTest.GetCollisionHitPhysicsDescriptors();
                std::vector<int32_t> descriptorActorIds;
                std::transform(collidedDescriptors.begin(), collidedDescriptors.end(), std::back_inserter(descriptorActorIds),
                               [](const auto &collidedDescriptor)
                               { return collidedDescriptor->GetOwnerActorEngineObjectId(); });

                if (descriptorActorIds.size())
                {
                    const auto &spaceStationTranslation = spaceStation->GetRootComponent()->GetTranslation();
                    const auto foundNearestIt = std::min_element(descriptorActorIds.begin(), descriptorActorIds.end(), [this, spaceStationTranslation](const auto &leftActorId, const auto &rightActorId)
                                                                 {
                        const auto &leftShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(leftActorId);
                        const auto &rightShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(rightActorId);
                        assert(leftShipActor && rightShipActor);
                        const auto sqrDistanceToLeft = glm::distance2(leftShipActor->GetRootComponent()->GetTranslation(), spaceStationTranslation);
                        const auto sqrDistanceToRight = glm::distance2(rightShipActor->GetRootComponent()->GetTranslation(), spaceStationTranslation);
                        return sqrDistanceToLeft < sqrDistanceToRight; });
                    if (foundNearestIt != descriptorActorIds.end())
                    {
                        const auto gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(*foundNearestIt);
                        assert(eGameObjectsType::SPACESHIP == gameObjectType);
                        const auto &nearestEnemy = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(*foundNearestIt);
                        const auto &enemyPosition = nearestEnemy->GetRootComponent()->GetTranslation();
                        const auto &projectileShootDirection = glm::normalize(enemyPosition - spaceStationTranslation);

                        const auto getRandomMissileType = [this]()
                        {
                            const auto missileValue = glm::clamp(static_cast<int32_t>(Random::Float() * 5.0), 1, 4);
                            const auto missileType = static_cast<eMissileType>(missileValue);
                            return mCombatActorsPoolHandler->GetFreeMissile(missileType) ? missileType : eMissileType::NONE;
                        };
                        eMissileType missileType = eMissileType::NONE;
                        while (eMissileType::NONE == missileType)
                        {
                            missileType = getRandomMissileType();
                        }

                        LaunchMisile(spaceStation, spaceStationTranslation, projectileShootDirection, missileType);
                        spaceStation->RestartTimerSinceLastShoot();
                    }
                }
            }
        }
    }

    void CombatController::CleanUp()
    {
        if (mNavigationController)
        {
            mNavigationController->CleanUp();
            mNavigationController.reset();
        }

        if (mUserInteractionController)
        {
            mUserInteractionController->CleanUp();
            mUserInteractionController.reset();
        }

        if (mCombatActorsPoolHandler)
        {
            mCombatActorsPoolHandler->CleanUp();
            mCombatActorsPoolHandler.reset();
        }
    }
}