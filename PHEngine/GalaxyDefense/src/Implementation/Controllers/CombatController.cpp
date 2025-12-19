#include "CombatController.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/CollisionTestImplementation/SphereCollisionTestWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/Actors/BlackHoleMissileActor.h"
#include "Implementation/Actors/PortalActor.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/Levels/LevelSerializationHelper.h"
#include "Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h"
#include "Implementation/Modifiers/ElectroRayChainModifier.h"
#include "Implementation/Navigation/Path.h"
#include "Implementation/Navigation/PathSegment.h"

#include <json/json.hpp>

#include <array>
#include <tuple>
#include <unordered_map>

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;

#undef min // without it conflicts with math min, max function
#undef max

namespace Game {

CombatController::CombatController(const std::weak_ptr<Scene>& scene)
    : mScene(scene)
    , mLevelBounds(BoundingBox3D(glm::vec3(0), glm::vec3(100, 50, 100)))
    , mNavigationController(std::make_shared<NavigationController>(scene))
    , mUserInteractionController(std::make_shared<UserInteractionController>(scene))
    , mCombatActorsPoolHandler(std::make_shared<CombatActorsPoolHandler>(scene))
{
}

CombatController::~CombatController()
{
    ElectroRaySphereContactCollisionEvent::GetInstance()->RemoveListener(ElectroRaySphereContactCollisionEvent::GetInstanceId());
    PhysicsCollisionGameThreadEvent::GetInstance()->RemoveListener(PhysicsCollisionGameThreadEvent::GetInstanceId());
    ShootRayCollisionEvent::GetInstance()->RemoveListener(ShootRayCollisionEvent::GetInstanceId());
    BroadcastGameThreadEvent::GetInstance()->RemoveListener(BroadcastGameThreadEvent::GetInstanceId());
    ChangeGameModeEvent::GetInstance()->RemoveListener(ChangeGameModeEvent::GetInstance()->GetInstanceId());
}

void CombatController::OnPreLevelInit()
{
    const auto thisSp = shared_from_this();
    ElectroRaySphereContactCollisionEvent::GetInstance()->AddListener(thisSp);
    PhysicsCollisionGameThreadEvent::GetInstance()->AddListener(thisSp);
    ShootRayCollisionEvent::GetInstance()->AddListener(thisSp);
    BroadcastGameThreadEvent::GetInstance()->AddListener(thisSp);
    ChangeGameModeEvent::GetInstance()->AddListener(thisSp);
    mNavigationController->OnPreLevelInit();
    mUserInteractionController->OnPreLevelInit();
}

void CombatController::InitFromLevelData(const LevelData& levelData)
{
    const auto& sceneSp = mScene.lock();
    ext_assert(sceneSp, "Scene pointer is null in CombatController::InitFromLevelData");
    if (!levelData.isDataValid()) {
        return;
    }

    mLevelBounds = BoundingBox3D(
        glm::vec3(0.0f),
        glm::vec3(
            std::abs(levelData.LevelBoundaryMax.x - levelData.LevelBoundaryMin.x) * 0.5f,
            50.0f,
            std::abs(levelData.LevelBoundaryMax.y - levelData.LevelBoundaryMin.y) * 0.5));
    mNavigationController->SetLevelBounds(mLevelBounds);
    mUserInteractionController->SetLevelBounds(mLevelBounds);
    mUserInteractionController->SetTowersData(levelData.TowersData);

    std::unordered_map<std::string, Path> pathRoutes;

    for (const auto& [routeName, route] : levelData.RoutesData) {
        Path path;
        PathSegment segment;
        segment.SetSubdivisionsCount(50);
        for (const auto& routeControlPoint : route) {
            segment.SetControlPoints(
                {{std::get<0>(routeControlPoint), std::get<1>(routeControlPoint), std::get<2>(routeControlPoint)}});
            path.AppendPathSegmentToTheEnd(segment);
        }
        pathRoutes.emplace(routeName, path);
        pathRoutes[routeName].CalculateRoutePoints();
    }
    mNavigationController->SetPathRoutes(pathRoutes);

    ext_assert(pathRoutes.size() > 0, " No path routes found in level data in CombatController::InitFromLevelData");
    mCombatActorsPoolHandler->SpawnPortals(pathRoutes.size(), Game::Constants::c_portalSize);
    std::vector<glm::vec3> realPortalPositions;
    for (const auto& [pathName, pathRoute] : pathRoutes) {
        if (realPortalPositions.empty()) {
            realPortalPositions.emplace_back(pathRoute.GetRouteFirstPoint());
        } else {
            const auto& newPortalPos = pathRoute.GetRouteFirstPoint();
            const bool bIsTooCloseToAnotherPortal
                = std::any_of(realPortalPositions.cbegin(), realPortalPositions.cend(), [&](const auto& existingPortalPos) {
                      return glm::distance2(existingPortalPos, newPortalPos)
                          < (Game::Constants::c_portalSize * Game::Constants::c_portalSize);
                  });
            if (!bIsTooCloseToAnotherPortal) {
                realPortalPositions.emplace_back(newPortalPos);
            }
        }
    }

    for (const auto& portalPos : realPortalPositions) {
        const auto& portalSp = mCombatActorsPoolHandler->GetFreePortalActor();
        ext_assert(portalSp, "Failed to get free portal actor");
        portalSp->SetIsEnabled(true);
        portalSp->GetRootComponent()->SetTranslation(portalPos);
        portalSp->SetNavigationController(mNavigationController);
        portalSp->SetCombatActorsPoolsHandler(mCombatActorsPoolHandler);
    }

    for (const auto& [barrierName, barrierData] : levelData.BarriersData) {
        mCombatActorsPoolHandler->SpawnBarriers(1, barrierData.size());
        const auto& a_barrier = mCombatActorsPoolHandler->GetFreeBarrierActor();
        ext_assert(
            a_barrier && barrierData.size() == a_barrier->GetBarrierPillarsCount(),
            "Invalid barrier data in CombatController::InitFromLevelData");
        int32_t pillarIndex = 0;
        for (const auto& pillarPosition : barrierData) {
            a_barrier->TrySetBarrierPillarMeshRelativeTransform(
                pillarIndex++, pillarPosition, glm::vec3(), glm::vec3(6.0f, 12.0f, 6.0f));
        }
        a_barrier->SetIsEnabled(true);
    }

    for (const auto& [stationName, spaceStationData] : levelData.TowersData) {
        const auto& spaceStationSp = mCombatActorsPoolHandler->CreateSpaceStationActor(
            stationName, glm::vec3(), glm::vec3(), std::get<1>(spaceStationData));
        spaceStationSp->SetIsEnabled(false);
    }

    const int32_t c_bombMissilesCount = 5 * levelData.TowersData.size();
    mCombatActorsPoolHandler->SpawnMissiles(eMissileType::BOMB, c_bombMissilesCount);
    mCombatActorsPoolHandler->SpawnMissiles(eMissileType::FREEZING_BOMB, 2);
    mCombatActorsPoolHandler->SpawnMissiles(eMissileType::ELECTRO_RAY, 1);
    mCombatActorsPoolHandler->SpawnMissiles(eMissileType::BLACK_HOLE, 1);
    mCombatActorsPoolHandler->SpawnMissiles(eMissileType::FREEZING_RAY, 1);
}

void CombatController::OnLevelInit()
{
    mNavigationController->OnLevelInit();
    mUserInteractionController->SetActorsPoolHandler(mCombatActorsPoolHandler);
    mUserInteractionController->SetOnShootCallback(std::bind(&CombatController::OnReadyToShoot, this));
    mUserInteractionController->OnLevelInit();
}

void CombatController::OnPostLevelInit()
{
    mNavigationController->OnPostLevelInit();
    mUserInteractionController->OnPostLevelInit();

    const std::unordered_map<eMissileType, size_t> availabeMissileTypes
        = {{eMissileType::BOMB, 10},
           {eMissileType::FREEZING_BOMB, 3},
           {eMissileType::ELECTRO_RAY, 1},
           {eMissileType::BLACK_HOLE, 2}};
    PlayerDataProvider::GetInstance()->SetAvailableMissileTypes(availabeMissileTypes);

    mCombatActorsPoolHandler->SpawnEnemySpaceships(10);
    mCombatActorsPoolHandler->SpawnAsteroids(20);
    mCombatActorsPoolHandler->SpawnBarriers(1, 5);
    const auto& extendedLevelBoundaries = BoundingBox3D(mLevelBounds.GetOrigin(), mLevelBounds.GetHalfExtent() * 1.25f);
    const auto& lvlBoundaryMin = extendedLevelBoundaries.GetMin();
    const auto& lvlBoundaryMax = extendedLevelBoundaries.GetMax();
    const auto& lvlBoundaryOrigin = extendedLevelBoundaries.GetOrigin();
    if (const auto& a_barrierSp = mCombatActorsPoolHandler->GetFreeBarrierActor()) {
        a_barrierSp->SetIsEnabled(true);
        a_barrierSp->GetRootComponent()->SetTranslation(glm::vec3(0, lvlBoundaryOrigin.y, 0));
        a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(
            0, glm::vec3(lvlBoundaryMin.x, 0, lvlBoundaryMin.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
        a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(
            1, glm::vec3(lvlBoundaryMin.x, 0, lvlBoundaryMax.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
        a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(
            2, glm::vec3(lvlBoundaryMax.x, 0, lvlBoundaryMax.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
        a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(
            3, glm::vec3(lvlBoundaryMax.x, 0, lvlBoundaryMin.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
        a_barrierSp->TrySetBarrierPillarMeshRelativeTransform(
            4, glm::vec3(lvlBoundaryMin.x, 0, lvlBoundaryMin.z), glm::vec3(), glm::vec3(6.0, 12.0, 6.0));
    }
}

void CombatController::PostPlayLevelFinished()
{
    mNavigationController->PostPlayLevelFinished();
    mUserInteractionController->PostPlayLevelFinished();

    ChangeGameModeEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, eGameModeType::SPACE_STATION_PLACEMENT);
}

void CombatController::OnCombatPreparationCompleted()
{
    const auto& spawnPortals = mCombatActorsPoolHandler->GetPortalActors();
    const auto& pathNames = mNavigationController->GetPathNames();
    ext_assert(
        pathNames.size() >= spawnPortals.size(),
        "Not enough paths for spawn portals in CombatController::OnCombatPreparationCompleted");
    for (int i = 0; i < pathNames.size(); ++i) {
        const auto& pathName = pathNames[i];
        const Path& path = mNavigationController->GetPath(pathName);
        const auto& spawnPortal = spawnPortals[i];
        spawnPortal->SetupSpaceshipSpawn(pathName, Game::Constants::c_spawnSpaceshipTimeoutMs);
        spawnPortal->StartSpawn();
        const auto& extendedPaths = mNavigationController->GetExtendedPaths();
        const auto& [extendedPathsBegin, extendedPathsEnd] = extendedPaths.equal_range(pathName);
        std::for_each(extendedPathsBegin, extendedPathsEnd, [&](const auto& extPath) {
            spawnPortal->SetupSpaceshipSpawn(extPath.second.first, Game::Constants::c_spawnSpaceshipTimeoutMs);
            spawnPortal->StartSpawn();
        });
    }
}

void CombatController::OnReadyToShoot()
{
    LogInfo("CombatController::OnReadyToShoot");
    const auto& projectileMarkerPosition = mUserInteractionController->GetProjectileMarkerPosition();
    const auto selectedSpaceStationId = mUserInteractionController->GetSelectedSpaceStationId();
    const auto& activeSpaceStationActor = mCombatActorsPoolHandler->GetSpaceStationOwnerActorById(selectedSpaceStationId);
    ext_assert(activeSpaceStationActor, "Active space station actor not found");
    const auto& activeSpaceStationPosition = activeSpaceStationActor->GetRootComponent()->GetTranslation();
    const auto& projectileShootDirection = glm::normalize(projectileMarkerPosition - activeSpaceStationPosition);
    const auto selectedMissileType = PlayerDataProvider::GetInstance()->GetSelectedMissileType();
    LaunchMisile(activeSpaceStationActor, activeSpaceStationPosition, projectileShootDirection, selectedMissileType);
}

void CombatController::ProcessEvent(
    const PhysicsCollisionGameThreadEvent* sender, const typename PhysicsCollisionGameThreadEvent::EventData_t& data)
{
    const ePhysicsCollisionStateType collisionEventType = std::get<0>(data);
    const ePhysicsBodyType physBodyType = std::get<1>(data);
    const auto this_actor_id = std::get<3>(data);
    const auto that_actor_id = std::get<5>(data);

    const auto& sceneSp = mScene.lock();
    if (!sceneSp || ePhysicsBodyType::GHOST != physBodyType)
        return;

    const std::string collisionType = ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType
        ? "collision registered"
        : "collision unregister";

    const auto thisActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(this_actor_id);
    const auto thatActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(that_actor_id);
    const auto objectsCollisionType
        = mCombatActorsPoolHandler->GetGameObjectsCollisionType(thisActorGameObjectType, thatActorGameObjectType);

    if (objectsCollisionType == eGameObjectsCollisionType::UNDEFINED)
        return;
    if (eGameObjectsCollisionType::SPACESHIP_WITH_MISSILE == objectsCollisionType) {
        const auto& ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType
            ? mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(this_actor_id)
            : mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(that_actor_id);

        const auto& ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType
            ? mCombatActorsPoolHandler->GetMissileOwnerActorById(this_actor_id)
            : mCombatActorsPoolHandler->GetMissileOwnerActorById(that_actor_id);

        const auto spaceshipActor_id = eGameObjectsType::SPACESHIP == thisActorGameObjectType ? this_actor_id : that_actor_id;
        const auto missileActor_id = eGameObjectsType::MISSILE == thisActorGameObjectType ? this_actor_id : that_actor_id;

        const auto& concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
            ? ownerMissileActor
            : ownerMissileActor->GetChildByObjectId(missileActor_id);

        const auto& concreteSpaceshipActor = ownerEnemyShipActor->GetObjectId() == spaceshipActor_id
            ? ownerEnemyShipActor
            : ownerEnemyShipActor->GetChildByObjectId(spaceshipActor_id);

        LogInfo(
            "CombatController::PhysicsCollisionGameThreadEvent:",
            collisionType,
            "missile with spaceship, this_actor = ",
            concreteMissileActor->GetName(),
            " that_actor = ",
            concreteSpaceshipActor->GetName());

        const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
        if (ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType) {
            explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor);
        } else {
            explosionVisitor->EndExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor);
        }
    } else if (eGameObjectsCollisionType::SPACESHIP_WITH_NEUTRAL_SPACE_OBJECT == objectsCollisionType) {
        const auto& ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType
            ? mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(this_actor_id)
            : mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(that_actor_id);
        const auto& ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType
            ? mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(this_actor_id)
            : mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(that_actor_id);

        LogInfo(
            "CombatController::PhysicsCollisionGameThreadEvent:",
            collisionType,
            "spaceship with space object, this_actor = ",
            ownerEnemyShipActor->GetName(),
            " that_actor = ",
            ownerSpaceObjectActor->GetName());

        ownerSpaceObjectActor->TriggerDisabled();
        ownerEnemyShipActor->TriggerDamageReceived(1UL, eDamageDealerType::NEUTRAL_OBJECT);
    } else if (eGameObjectsCollisionType::MISSILE_WITH_NEUTRAL_SPACE_OBJECT == objectsCollisionType) {
        const auto& ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType
            ? mCombatActorsPoolHandler->GetMissileOwnerActorById(this_actor_id)
            : mCombatActorsPoolHandler->GetMissileOwnerActorById(that_actor_id);

        const auto& ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType
            ? mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(this_actor_id)
            : mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(that_actor_id);
        const auto& missileActor_id = eGameObjectsType::MISSILE == thisActorGameObjectType ? this_actor_id : that_actor_id;

        const auto& concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
            ? ownerMissileActor
            : ownerMissileActor->GetChildByObjectId(missileActor_id);

        LogInfo(
            "CombatController::PhysicsCollisionGameThreadEvent: ",
            collisionType,
            "missile with space object, this_actor = ",
            concreteMissileActor->GetName(),
            " that_actor = ",
            ownerSpaceObjectActor->GetName());

        const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
        if (ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType) {
            explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor);
        } else {
            explosionVisitor->EndExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor);
        }
    }
}

void CombatController::ProcessEvent(
    const ShootRayCollisionEvent* sender, const typename ShootRayCollisionEvent::EventData_t& data)
{
    const auto& eventSenderMissileWp = std::get<0>(data);
    const auto& collidedActorWp = std::get<1>(data);
    const auto& rayType = std::get<2>(data);
    const auto& collisionActionType = std::get<3>(data);

    if (const auto& rayMissileActorSp = eventSenderMissileWp.lock()) {
        if (const auto& collidedActorSp = collidedActorWp.lock()) {
            const auto& gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedActorSp->GetObjectId());
            if (eGameObjectsType::UNDEFINED != gameObjectType) {
                const auto explosionVisitor = rayMissileActorSp->CreateMissileExplosionVisitor();
                if (eCollisionActionType::COLLISION_STARTED == collisionActionType) {
                    if (eGameObjectsType::SPACESHIP == gameObjectType) {
                        const auto& ownerEnemyShipActor
                            = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, rayMissileActorSp);
                    } else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType) {
                        const auto& ownerSpaceObjectActor
                            = mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, rayMissileActorSp);
                    }
                } else if (eCollisionActionType::COLLISION_FINISHED == collisionActionType) {
                    if (eGameObjectsType::SPACESHIP == gameObjectType) {
                        const auto& ownerEnemyShipActor
                            = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->EndExplosionForSpaceship(ownerEnemyShipActor, rayMissileActorSp);
                    } else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType) {
                        const auto& ownerSpaceObjectActor
                            = mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->EndExplosionForSpaceObject(ownerSpaceObjectActor, rayMissileActorSp);
                    }
                }
            }
        }
    }
}

void CombatController::ProcessEvent(
    const ElectroRaySphereContactCollisionEvent* sender, const typename ElectroRaySphereContactCollisionEvent::EventData_t& data)
{
    const auto& srcActorId = std::get<0>(data);
    const auto& collidedActorIds = std::move(std::get<1>(data));

    const auto& srcActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(srcActorId);
    if (eGameObjectsType::UNDEFINED != srcActorGameObjectType && eGameObjectsType::MISSILE != srcActorGameObjectType) {
        const std::shared_ptr<Actor>& srcCollisionActor = eGameObjectsType::SPACESHIP == srcActorGameObjectType
            ? std::static_pointer_cast<Actor>(mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(srcActorId))
            : eGameObjectsType::NEUTRAL_SPACE_OBJECT == srcActorGameObjectType
                ? std::static_pointer_cast<Actor>(mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(srcActorId))
                : nullptr;
        ext_assert(srcCollisionActor, "Source collision actor not found");
        for (const auto& collidedActorId : collidedActorIds) {
            const auto& gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedActorId);
            if (eGameObjectsType::UNDEFINED != gameObjectType) {
                if (eGameObjectsType::SPACESHIP == gameObjectType) {
                    const auto& ownerEnemyShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(collidedActorId);
                    if (ownerEnemyShipActor->IsEnabled() && ownerEnemyShipActor->IsVisible()) {
                        const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(
                            std::make_pair(gameObjectType, ownerEnemyShipActor),
                            std::make_pair(srcActorGameObjectType, srcCollisionActor));
                        electroRayChainModifier->Initialize(mCombatActorsPoolHandler->GetFreeElectroChainActor());
                        ownerEnemyShipActor->AddModifier(electroRayChainModifier);
                    }
                } else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType) {
                    const auto& ownerSpaceObjectActor = mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(collidedActorId);
                    const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(
                        std::make_pair(gameObjectType, ownerSpaceObjectActor),
                        std::make_pair(srcActorGameObjectType, srcCollisionActor));

                    if (ownerSpaceObjectActor->IsEnabled() && ownerSpaceObjectActor->IsVisible()) {
                        electroRayChainModifier->Initialize(mCombatActorsPoolHandler->GetFreeElectroChainActor());
                        ownerSpaceObjectActor->AddModifier(electroRayChainModifier);
                    }
                }
            }
        }
    }
}

void CombatController::ProcessEvent(
    const BroadcastGameThreadEvent* sender, const typename BroadcastGameThreadEvent::EventData_t& data)
{
    const auto& eventHeader = std::get<0>(data);
    const auto& jsonParams = std::get<1>(data);
    if ("CombatLevelEvents" == eventHeader) {
        const auto& jsonObj = nlohmann::json::parse(std::get<1>(data));
        const auto& actionName = jsonObj.at("action").get<std::string>();
        if ("button_press" == actionName) {
            const auto selectedSpaceStationId = mUserInteractionController->GetSelectedSpaceStationId();
            if (selectedSpaceStationId >= 0) {
                const auto& buttonType = jsonObj.at("button_type").get<std::string>();
                static std::unordered_map<std::string, eMissileType> missilesMap
                    = {{"Bomb", eMissileType::BOMB},
                       {"Freezing", eMissileType::FREEZING_BOMB},
                       {"Electro_Ray", eMissileType::ELECTRO_RAY},
                       {"Black_Hole", eMissileType::BLACK_HOLE}};
                ext_assert(missilesMap.count(buttonType), "Unknown button type: " + buttonType);
                PlayerDataProvider::GetInstance()->SetSelectedMissileType(missilesMap.at(buttonType));
                mUserInteractionController->ShowMissileProjectile();
            }
        }
    }
}

void CombatController::ProcessEvent(const ChangeGameModeEvent* sender, const typename ChangeGameModeEvent::EventData_t& data)
{
    const auto newValue = std::get<0>(data);
    if (newValue != mGameModeType) {
        static constexpr auto functionId = Hash64_CT("CombatController::LuaChangeGameModeEvent");
        if (eGameModeType::INIT == mGameModeType && eGameModeType::SPACE_STATION_PLACEMENT == newValue) {
            if (const auto& sceneSp = mScene.lock()) {
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
                    static_cast<int32_t>(eGameModeType::SPACE_STATION_PLACEMENT),
                    functionId,
                    [](std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                       std::weak_ptr<EngineCore::Scene> sceneWp,
                       std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        LuaChangeGameModeEvent::GetInstance()->SendEvent(
                            eExecutionOrder::POST_EXECUTION, eGameModeType::SPACE_STATION_PLACEMENT);
                    });
            }
        } else if (eGameModeType::SPACE_STATION_PLACEMENT == mGameModeType && eGameModeType::COMBAT == newValue) {
            if (const auto& sceneSp = mScene.lock()) {
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
                    static_cast<int32_t>(eGameModeType::COMBAT),
                    functionId,
                    [](std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                       std::weak_ptr<EngineCore::Scene> sceneWp,
                       std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        LuaChangeGameModeEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, eGameModeType::COMBAT);
                    });
                OnCombatPreparationCompleted();
            }
        }

        mGameModeType = newValue;
    }
}

void CombatController::Tick(const float deltaTimeSec)
{
    if (eGameModeType::COMBAT == mGameModeType) {
        ValidatePoolObjects();
        UpdateMissilesData();
        ProcessAiAction();
        mNavigationController->Tick(deltaTimeSec);
    }

    mUserInteractionController->Tick(deltaTimeSec);
}

void CombatController::LaunchMisile(
    const std::shared_ptr<SpaceStationActor>& missileOwner,
    const glm::vec3& missileStartPosition,
    const glm::vec3& missileDirection,
    const eMissileType missileType)
{
    const auto& missile = mCombatActorsPoolHandler->GetFreeMissile(missileType);
    ext_assert(missile, "Failed to get free missile from pool");
    const auto yawRad = std::atan2(missileDirection.x, missileDirection.z);
    const auto yawDeg = RAD_TO_DEG(yawRad);
    missile->TriggerSpawn(missileStartPosition, missileDirection, yawDeg, eDamageDealerType::MAIN_PLAYER, missileOwner);
    mNavigationController->PutMissileToNavigate(missile);
}

void CombatController::ValidatePoolObjects()
{
    const auto& enemySpaceshipActors = mCombatActorsPoolHandler->GetEnemySpaceshipActors();

    if (enemySpaceshipActors.size()) {
        int32_t returnedToPoolSpaceships = 0;
        for (const auto& enemySpaceship : enemySpaceshipActors) {
            if (eSpaceshipActivityState::PENDING_DISABLE == enemySpaceship->GetSpaceshipActivityState()) {
                mNavigationController->RemoveSpaceshipFromRoute(enemySpaceship->GetObjectId());
                enemySpaceship->TriggerDisabled();
                ++returnedToPoolSpaceships;
            }
        }
        if (returnedToPoolSpaceships) {
            LogInfo("CombatController::ValidatePoolObjects: returnedToPoolSpaceships: ", returnedToPoolSpaceships);
        }
    }

    const auto& missileActors = mCombatActorsPoolHandler->GetMissileActors();

    if (missileActors.size()) {
        int32_t returnedToPoolMissiles = 0;
        for (const auto& missileActor : missileActors) {
            if (eMissileActivityState::OUT_OF_LEVEL == missileActor->GetMissileActivityState()) {
                mNavigationController->RemoveMissileFromNavigation(missileActor->GetObjectId());
                missileActor->TriggerDisabled();
                ++returnedToPoolMissiles;
            } else if (eMissileActivityState::IDLE == missileActor->GetMissileActivityState()) {
                mNavigationController->RemoveMissileFromNavigation(missileActor->GetObjectId());
            }
        }
        if (returnedToPoolMissiles) {
            LogInfo("CombatController::ValidatePoolObjects: returnedToPoolMissiles: ", returnedToPoolMissiles);
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
    const auto& sceneSp = mScene.lock();
    if (!sceneSp) {
        return;
    }

    const auto& spaceStations = mCombatActorsPoolHandler->GetSpaceStationActors();
    std::vector<std::shared_ptr<PhysicsComponent>> excludedPhysicsComponents;
    const auto& spaceStationsPhysComponents = mCombatActorsPoolHandler->GetSpaceStationsPhysicsComponents();
    const auto& bombMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BOMB);
    const auto& freezeMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::FREEZING_BOMB);
    const auto& blackHoleMissilePhysComponents = mCombatActorsPoolHandler->GetMissilePhysicsComponents(eMissileType::BLACK_HOLE);
    excludedPhysicsComponents.reserve(spaceStations.size() + mCombatActorsPoolHandler->GetMissileActors().size());
    excludedPhysicsComponents.insert(
        excludedPhysicsComponents.end(), spaceStationsPhysComponents.begin(), spaceStationsPhysComponents.end());
    excludedPhysicsComponents.insert(
        excludedPhysicsComponents.end(), bombMissilePhysComponents.begin(), bombMissilePhysComponents.end());
    excludedPhysicsComponents.insert(
        excludedPhysicsComponents.end(), freezeMissilePhysComponents.begin(), freezeMissilePhysComponents.end());
    excludedPhysicsComponents.insert(
        excludedPhysicsComponents.end(), blackHoleMissilePhysComponents.begin(), blackHoleMissilePhysComponents.end());

    for (const auto& spaceStation : spaceStations) {
        if (eSpaceStationActivityState::ACTIVE == spaceStation->GetState() && spaceStation->CanShoot()) {
            SphereCollisionTestWithFilterAdapter collisionTest(
                spaceStation->GetSpaceStationLevel()->GetShootRadius(), excludedPhysicsComponents);
            collisionTest.SphereCollisionTest(sceneSp->GetPhysicsWorld(), spaceStation->GetRootComponent()->GetTranslation());
            const auto& collidedDescriptors = collisionTest.GetCollisionHitPhysicsDescriptors();
            std::vector<int32_t> descriptorActorIds;
            std::transform(
                collidedDescriptors.begin(),
                collidedDescriptors.end(),
                std::back_inserter(descriptorActorIds),
                [](const auto& collidedDescriptor) { return collidedDescriptor->GetOwnerActorEngineObjectId(); });

            if (descriptorActorIds.size()) {
                const auto& spaceStationTranslation = spaceStation->GetRootComponent()->GetTranslation();
                const auto foundNearestIt = std::min_element(
                    descriptorActorIds.begin(),
                    descriptorActorIds.end(),
                    [this, spaceStationTranslation](const auto& leftActorId, const auto& rightActorId) {
                        const auto& leftShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(leftActorId);
                        const auto& rightShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(rightActorId);
                        ext_assert(leftShipActor && rightShipActor, "Failed to find enemy ship actors for distance comparison");
                        const auto sqrDistanceToLeft
                            = glm::distance2(leftShipActor->GetRootComponent()->GetTranslation(), spaceStationTranslation);
                        const auto sqrDistanceToRight
                            = glm::distance2(rightShipActor->GetRootComponent()->GetTranslation(), spaceStationTranslation);
                        return sqrDistanceToLeft < sqrDistanceToRight;
                    });
                if (foundNearestIt != descriptorActorIds.end()) {
                    const auto gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(*foundNearestIt);
                    ext_assert(eGameObjectsType::SPACESHIP == gameObjectType, "Expected game object type to be SPACESHIP");
                    const auto& nearestEnemy = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(*foundNearestIt);
                    const auto& enemyPosition = nearestEnemy->GetRootComponent()->GetTranslation();
                    const auto& projectileShootDirection = glm::normalize(enemyPosition - spaceStationTranslation);
                    LaunchMisile(
                        spaceStation,
                        spaceStationTranslation,
                        projectileShootDirection,
                        spaceStation->GetSpaceStationLevel()->GetMissileType());
                    spaceStation->RestartTimerSinceLastShoot();
                }
            }
        }
    }
}

void CombatController::CleanUp()
{
    if (mNavigationController) {
        mNavigationController->CleanUp();
        mNavigationController.reset();
    }

    if (mUserInteractionController) {
        mUserInteractionController->CleanUp();
        mUserInteractionController.reset();
    }

    if (mCombatActorsPoolHandler) {
        mCombatActorsPoolHandler->CleanUp();
        mCombatActorsPoolHandler.reset();
    }
}
} // namespace Game