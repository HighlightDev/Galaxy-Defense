#include "NavigationController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/DataProviders/LevelDataProvider.h"

#include <glm/vec3.hpp>

#include <array>

using namespace Graphics;
using namespace Resources;

namespace Game {
NavigationController::NavigationController(const std::weak_ptr<::EngineCore::Scene>& sceneWp)
    : mSceneWp(sceneWp)
    , mNavPathDummyActor(std::make_shared<Actor>(
          "NavPathDummyActor",
          std::make_shared<EngineCore::SceneComponent>(
              "NavPathDummy_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1), true)))
    , mBarriersController(sceneWp)
{
}

void NavigationController::Initialize()
{
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in NavigationController::Initialize");
    sceneSp->AddActor(mNavPathDummyActor);
}

void NavigationController::SetFinalDestinationPoint(const glm::vec3& destinationPoint)
{
    mFinalDestinationPoint = destinationPoint;
}

void NavigationController::SetPortalPositions(const std::vector<glm::vec3>& portalPositions)
{
    mPortalPositions = portalPositions;
}

bool NavigationController::IsPositionNearFinalDestination(const glm::vec3& position, const float radius) const
{
    const float dist
        = glm::distance(glm::vec2(position.x, position.z), glm::vec2(mFinalDestinationPoint.x, mFinalDestinationPoint.z));
    return dist < radius;
}

void NavigationController::SetLevelBounds(const BoundingBox3D& levelBounds)
{
    mLevelBounds = levelBounds;
    InitializeNavMesh();
}

void NavigationController::OnPreLevelInit()
{
}

void NavigationController::OnLevelInit()
{
    Initialize();
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        InitializeNavMeshDebugRendering();
        InitializeDebugPathRendering();
    }
#endif
}

void NavigationController::OnPostLevelInit()
{
}

void NavigationController::PostPlayLevelFinished()
{
}

void NavigationController::CleanUp()
{
    mEnemies.clear();
    mMissiles.clear();
}

void NavigationController::Tick(const float deltaTimeSec)
{
    bool needToValidateMissiles = false;
    for (auto& missile : mMissiles) {
        if (!missile) {
            needToValidateMissiles = true;
        } else if (
            eMissileActivityState::IDLE != missile->GetMissileActivityState()
            && eMissileActivityState::OUT_OF_LEVEL != missile->GetMissileActivityState()) {
            if (!missile->IsInsideLevel(mLevelBounds)) {
                missile->SetMissileActivityState(eMissileActivityState::OUT_OF_LEVEL);
                LogInfo("NavigationController::Tick: missile ", missile->GetObjectId(), " is out of level.");
            }
        }
    }

    if (needToValidateMissiles) {
        mMissiles.erase(std::remove_if(mMissiles.begin(), mMissiles.end(), [](const auto& missile) { return !missile; }));
    }

    for (const auto& spaceship : mEnemies) {
        const auto& routeMoveComp = spaceship->GetOnRouteMovementComponent();
        if (routeMoveComp->GetIsDistanceCompleted()) {
            const auto& levelDataProviderPtr = LevelDataProvider::GetInstance();
            levelDataProviderPtr->SetCurrentStageSurvivedEnemySpaceshipsCount(
                levelDataProviderPtr->GetCurrentStageSurvivedEnemySpaceshipsCount() + 1);
            spaceship->SetSpaceshipActivityState(eSpaceshipActivityState::PENDING_DISABLE);
            LogInfo("NavigationController::Tick: spaceship ", spaceship->GetObjectId(), " reached destination.");
        }
    }
}

void NavigationController::UnpausableTick(const float deltaTimeSec)
{
}

std::vector<glm::vec3> NavigationController::BuildNavMeshRoute(const glm::vec3& startPosition) const
{
    ext_assert(mNavMesh, "NavMesh is null in NavigationController::BuildNavMeshRoute");
    const glm::vec2 start2D(startPosition.x, startPosition.z);
    const glm::vec2 end2D(mFinalDestinationPoint.x, mFinalDestinationPoint.z);
    const auto route2D = mNavMesh->BuildRouteBetweenPoints(start2D, end2D);
    std::vector<glm::vec3> route3D;
    route3D.reserve(route2D.size());
    for (const auto& point : route2D) {
        route3D.emplace_back(point.x, startPosition.y, point.y);
    }
    return route3D;
}

void NavigationController::PutSpaceshipOnRoute(const glm::vec3& startPosition, const std::shared_ptr<SpaceshipActor>& spaceship)
{
    LogInfo(
        "NavigationController::PutSpaceshipOnRoute: putting spaceship ",
        spaceship->GetObjectId(),
        " on route from position: ",
        startPosition);
    const auto route = BuildNavMeshRoute(startPosition);
    ext_assert(!route.empty(), "Failed to build NavMesh route in NavigationController::PutSpaceshipOnRoute");
    const auto enemyMovementComponent = spaceship->GetOnRouteMovementComponent();
    ext_assert(enemyMovementComponent, "Enemy movement component is null");
    enemyMovementComponent->ResetStates();
    enemyMovementComponent->SetIsMovementOnRouteAllowed(true);
    enemyMovementComponent->SetRoutePoints(route);
    spaceship->TriggerSpawn(route.front());
    mEnemies.emplace_back(spaceship);
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        CreateDebugPathForSpaceship(spaceship->GetObjectId(), route);
    }
#endif
}

void NavigationController::RebuildActiveShipRoutes()
{
    for (const auto& spaceship : mEnemies) {
        if (eSpaceshipActivityState::ACTIVE != spaceship->GetSpaceshipActivityState()) {
            continue;
        }
        const auto& routeMoveComp = spaceship->GetOnRouteMovementComponent();
        if (!routeMoveComp || routeMoveComp->GetIsDistanceCompleted()) {
            continue;
        }
        const auto currentPosition = spaceship->GetWorldPosition();
        const auto newRoute = BuildNavMeshRoute(currentPosition);
        if (newRoute.size() > 1) {
            routeMoveComp->ReplaceRouteFromCurrentPosition(newRoute);
#ifdef DEBUG
            if (cEnableDebugPathRendering) {
                CreateDebugPathForSpaceship(spaceship->GetObjectId(), newRoute);
            }
#endif
        } else {
            // Ship is already at the destination cell — mark route as completed
            routeMoveComp->SetIsDistanceCompleted(true);
            continue;
        }
    }
}

void NavigationController::ReapplyAllObstaclesToNavMesh()
{
    mNavMesh->ResetAllCellsWalkable();

    for (const auto& barrierWp : mActiveBarriersOnLevel) {
        if (const auto barrier = barrierWp.lock()) {
            const auto& rayPositions = barrier->GetBarrierActiveRaysWorldPositions();
            for (const auto& [startPosition, endPosition] : rayPositions) {
                mNavMesh->FillCellStatesBetweenWorldPositions(
                    glm::vec2(startPosition.x, startPosition.z), glm::vec2(endPosition.x, endPosition.z), false);
            }
        }
    }

    for (const auto& stationWp : mActiveSpaceStationsOnLevel) {
        if (const auto station = stationWp.lock()) {
            MarkSpaceStationCellsOnNavMesh(station, false);
        }
    }
}

void NavigationController::MarkSpaceStationCellsOnNavMesh(
    const std::shared_ptr<SpaceStationActor>& spaceStationActor, const bool isWalkable)
{
    const auto& pos = spaceStationActor->GetRootComponent()->GetTranslation();
    const glm::vec2 center(pos.x, pos.z);
    const float halfNavCell = mNavMesh->GetCellSize() * 0.5f;

    mNavMesh->SetCellStateByWorldPosition(center + glm::vec2(-halfNavCell, -halfNavCell), isWalkable);
    mNavMesh->SetCellStateByWorldPosition(center + glm::vec2(+halfNavCell, -halfNavCell), isWalkable);
    mNavMesh->SetCellStateByWorldPosition(center + glm::vec2(-halfNavCell, +halfNavCell), isWalkable);
    mNavMesh->SetCellStateByWorldPosition(center + glm::vec2(+halfNavCell, +halfNavCell), isWalkable);
}

void NavigationController::PutMissileToNavigate(const std::shared_ptr<MissileActor>& missile)
{
    ext_assert(missile, "Missile pointer is null in PutMissileToNavigate");
    ext_assert(
        missile->GetMissileActivityState() == eMissileActivityState::ACTIVE, "Missile is not active in PutMissileToNavigate");
    const bool missingMissile = std::none_of(mMissiles.cbegin(), mMissiles.cend(), [missile](const auto& missileSp) {
        return missile->GetObjectId() == missileSp->GetObjectId();
    });
    if (missingMissile) {
        mMissiles.emplace_back(missile);
    }
}

void NavigationController::RemoveSpaceshipFromRoute(const int32_t spaceshipActorId)
{
    LogInfo("NavigationController::RemoveSpaceshipFromRoute: removing spaceship with id ", spaceshipActorId, " from route");
    if (mEnemies.size()) {
        mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(), [spaceshipActorId](const auto& enemy) {
            return spaceshipActorId == enemy->GetObjectId();
        }));
    }
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        RemoveDebugPathForSpaceship(spaceshipActorId);
    }
#endif
}

void NavigationController::RemoveMissileFromNavigation(const int32_t missileActorId)
{
    if (mMissiles.size()) {
        auto removeIt = std::remove_if(mMissiles.begin(), mMissiles.end(), [missileActorId](const auto& missile) {
            return !missile || (missile && missileActorId == missile->GetObjectId());
        });
        if (removeIt != mMissiles.end()) {
            mMissiles.erase(removeIt, mMissiles.end());
        }
    }
}

BarriersController& NavigationController::GetBarriersController()
{
    return mBarriersController;
}

const BarriersController& NavigationController::GetBarriersController() const
{
    return mBarriersController;
}

void NavigationController::InitializeNavMesh()
{
    constexpr float s_gridCellSizeForRoute = 5.0f;

    BoundingBox2D<glm::vec2> levelBoundingBox2D(
        glm::vec2(mLevelBounds.GetOrigin().x, mLevelBounds.GetOrigin().z),
        glm::vec2(mLevelBounds.GetHalfExtent().x, mLevelBounds.GetHalfExtent().z));
    mNavMesh = std::make_unique<EngineCore::NavigationMesh::NavMesh2D>(levelBoundingBox2D, s_gridCellSizeForRoute);
}

void NavigationController::PutActiveBarrierOnLevel(const std::shared_ptr<BarrierActor>& barrierActor)
{
    const auto seekBarrierIt = std::find_if(
        mActiveBarriersOnLevel.cbegin(), mActiveBarriersOnLevel.cend(), [barrierActor](const auto& barrierActorWp) {
            const auto barrierActorSp = barrierActorWp.lock();
            return barrierActorSp && barrierActor->GetObjectId() == barrierActorSp->GetObjectId();
        });
    if (seekBarrierIt == mActiveBarriersOnLevel.cend()) {
        mActiveBarriersOnLevel.emplace_back(barrierActor);
    }

    const auto& barrierRaysWorldPositions = barrierActor->GetBarrierActiveRaysWorldPositions();
    for (const auto& [startPosition, endPosition] : barrierRaysWorldPositions) {
        mNavMesh->FillCellStatesBetweenWorldPositions(
            glm::vec2(startPosition.x, startPosition.z), glm::vec2(endPosition.x, endPosition.z), false);
    }
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        RefreshNavMeshDebugRendering();
    }
#endif
    RebuildActiveShipRoutes();
}

void NavigationController::RemoveActiveBarrierFromLevel(const std::shared_ptr<BarrierActor>& barrierActor)
{
    mActiveBarriersOnLevel.erase(
        std::remove_if(
            mActiveBarriersOnLevel.begin(),
            mActiveBarriersOnLevel.end(),
            [barrierActor](const auto& barrierActorWp) {
                const auto barrierActorSp = barrierActorWp.lock();
                return !barrierActorSp || (barrierActorSp && barrierActor->GetObjectId() == barrierActorSp->GetObjectId());
            }),
        mActiveBarriersOnLevel.end());

    ReapplyAllObstaclesToNavMesh();
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        RefreshNavMeshDebugRendering();
    }
#endif
    RebuildActiveShipRoutes();
}

void NavigationController::PutActiveSpaceStationOnLevel(const std::shared_ptr<SpaceStationActor>& spaceStationActor)
{
    const auto seekIt = std::find_if(
        mActiveSpaceStationsOnLevel.cbegin(), mActiveSpaceStationsOnLevel.cend(), [spaceStationActor](const auto& stationWp) {
            const auto stationSp = stationWp.lock();
            return stationSp && spaceStationActor->GetObjectId() == stationSp->GetObjectId();
        });
    if (seekIt == mActiveSpaceStationsOnLevel.cend()) {
        mActiveSpaceStationsOnLevel.emplace_back(spaceStationActor);
    }

    MarkSpaceStationCellsOnNavMesh(spaceStationActor, false);
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        RefreshNavMeshDebugRendering();
    }
#endif
    RebuildActiveShipRoutes();
}

void NavigationController::RemoveActiveSpaceStationFromLevel(const std::shared_ptr<SpaceStationActor>& spaceStationActor)
{
    mActiveSpaceStationsOnLevel.erase(
        std::remove_if(
            mActiveSpaceStationsOnLevel.begin(),
            mActiveSpaceStationsOnLevel.end(),
            [spaceStationActor](const auto& stationWp) {
                const auto stationSp = stationWp.lock();
                return !stationSp || (stationSp && spaceStationActor->GetObjectId() == stationSp->GetObjectId());
            }),
        mActiveSpaceStationsOnLevel.end());

    ReapplyAllObstaclesToNavMesh();
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        RefreshNavMeshDebugRendering();
    }
#endif
    RebuildActiveShipRoutes();
}

#ifdef DEBUG
void NavigationController::InitializeNavMeshDebugRendering()
{
    const auto sceneSp = mSceneWp.lock();
    if (!sceneSp || !mNavMesh) {
        LogInfo("NavigationController::InitializeNavMeshDebugRendering: cannot initialize nav mesh debug rendering, because "
                "scene pointer or nav mesh is null");
        return;
    }

    mNavMeshDebugActor = std::make_shared<Actor>(
        "NavMeshDebugActor",
        std::make_shared<EngineCore::SceneComponent>("NavMeshDebug_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1), true));
    sceneSp->AddActor(mNavMeshDebugActor);

    MaterialParser materialParser;

    const auto& greenMaterial = materialParser.ParseMaterialDescriptor("AlbedoColorWithOpacityMaterial.m");
    sceneSp->RegisterMaterialInstance(greenMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(greenMaterial, "opacity", 0.35f);
    MaterialPropertySetter::SetMaterialPropertyValue(greenMaterial, "color", glm::vec3(0.0f, 0.8f, 0.0f));

    const auto& redMaterial = materialParser.ParseMaterialDescriptor("AlbedoColorWithOpacityMaterial.m");
    sceneSp->RegisterMaterialInstance(redMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(redMaterial, "opacity", 0.35f);
    MaterialPropertySetter::SetMaterialPropertyValue(redMaterial, "color", glm::vec3(0.8f, 0.0f, 0.0f));

    const auto& meshCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(false);

    const auto& levelMin = mNavMesh->GetLevelBoundingBox().GetMin();
    const float cellSize = mNavMesh->GetCellSize();
    const float cellScale = cellSize * 0.9f;

    const int32_t colsCount = mNavMesh->GetCellsCountX();
    const int32_t rowsCount = mNavMesh->GetCellsCountY();
    mNavMeshDebugGreenCells.resize(colsCount);
    mNavMeshDebugRedCells.resize(colsCount);

    for (int32_t x = 0; x < colsCount; ++x) {
        mNavMeshDebugGreenCells[x].resize(rowsCount);
        mNavMeshDebugRedCells[x].resize(rowsCount);

        for (int32_t y = 0; y < rowsCount; ++y) {
            const float worldX = levelMin.x + (static_cast<float>(x) + 0.5f) * cellSize;
            const float worldZ = levelMin.y + (static_cast<float>(y) + 0.5f) * cellSize;
            const glm::vec3 cellPos(worldX, 0.1f, worldZ);
            const bool isWalkable = mNavMesh->IsCellWalkable(x, y);
            const std::string suffix = std::to_string(x) + "_" + std::to_string(y);

            const auto& d_green = std::make_shared<MeshComponentData>(
                "navmesh_debug_green_" + suffix,
                "plane.obj",
                cellPos,
                glm::vec3(),
                glm::vec3(cellScale, 1.0f, cellScale),
                greenMaterial,
                isWalkable,
                isWalkable);
            const auto& c_green
                = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshCreator, d_green));
            c_green->SetSortOrderValue(0);
            mNavMeshDebugActor->AddComponent(c_green);
            mNavMeshDebugGreenCells[x][y] = c_green;

            const auto& d_red = std::make_shared<MeshComponentData>(
                "navmesh_debug_red_" + suffix,
                "plane.obj",
                cellPos,
                glm::vec3(),
                glm::vec3(cellScale, 1.0f, cellScale),
                redMaterial,
                !isWalkable,
                !isWalkable);
            const auto& c_red
                = std::static_pointer_cast<StaticMeshComponent>(sceneSp->CreateComponent_GameThread(meshCreator, d_red));
            c_red->SetSortOrderValue(0);
            mNavMeshDebugActor->AddComponent(c_red);
            mNavMeshDebugRedCells[x][y] = c_red;
        }
    }
}

void NavigationController::RefreshNavMeshDebugRendering()
{
    if (!mNavMesh) {
        return;
    }

    const int32_t colsCount = mNavMesh->GetCellsCountX();
    const int32_t rowsCount = mNavMesh->GetCellsCountY();

    for (int32_t x = 0; x < colsCount && x < static_cast<int32_t>(mNavMeshDebugGreenCells.size()); ++x) {
        for (int32_t y = 0; y < rowsCount && y < static_cast<int32_t>(mNavMeshDebugGreenCells[x].size()); ++y) {
            const bool isWalkable = mNavMesh->IsCellWalkable(x, y);

            if (const auto greenSp = mNavMeshDebugGreenCells[x][y].lock()) {
                greenSp->SetIsVisible(isWalkable);
                greenSp->SetIsEnabled(isWalkable);
            }
            if (const auto redSp = mNavMeshDebugRedCells[x][y].lock()) {
                redSp->SetIsVisible(!isWalkable);
                redSp->SetIsEnabled(!isWalkable);
            }
        }
    }
}

void NavigationController::InitializeDebugPathRendering()
{
    const auto sceneSp = mSceneWp.lock();
    if (!sceneSp) {
        return;
    }

    mDebugPathActor = std::make_shared<Actor>(
        "DebugPathActor",
        std::make_shared<EngineCore::SceneComponent>("DebugPath_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1), true));
    sceneSp->AddActor(mDebugPathActor);

    MaterialParser materialParser;
    mDebugPathMaterial = materialParser.ParseMaterialDescriptor("CurveLineMaterial.m");
    sceneSp->RegisterMaterialInstance(mDebugPathMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(mDebugPathMaterial, "opacity", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(mDebugPathMaterial, "color", glm::vec3(0.5f, 0.7f, 0.2f));
}

void NavigationController::CreateDebugPathForSpaceship(const int32_t spaceshipId, const std::vector<glm::vec3>& routePoints)
{
    const auto sceneSp = mSceneWp.lock();
    if (!sceneSp || !mDebugPathActor || !mDebugPathMaterial || routePoints.size() < 2) {
        return;
    }

    RemoveDebugPathForSpaceship(spaceshipId);

    const auto& meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();

    std::vector<std::shared_ptr<RuntimeGeneratedLineComponent>> lineSegments;
    lineSegments.reserve(routePoints.size() - 1);

    for (size_t i = 0; i + 1 < routePoints.size(); ++i) {
        auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
            "debug_path_" + std::to_string(spaceshipId) + "_seg_" + std::to_string(i),
            150,
            glm::vec3(),
            glm::vec3(),
            glm::vec3(1),
            mDebugPathMaterial,
            true,
            true);
        auto c_line = std::static_pointer_cast<RuntimeGeneratedLineComponent>(
            sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_line->SetLineWidth(0.5f);
        c_line->SetSortOrderValue(100);
        c_line->SetLineBeginWorldSpacePosition(routePoints[i] + glm::vec3(0.0f, 0.3f, 0.0f));
        c_line->SetLineEndWorldSpacePosition(routePoints[i + 1] + glm::vec3(0.0f, 0.3f, 0.0f));
        mDebugPathActor->AddComponent(c_line);
        lineSegments.emplace_back(c_line);
    }

    mDebugPathLines[spaceshipId] = std::move(lineSegments);
}

void NavigationController::RemoveDebugPathForSpaceship(const int32_t spaceshipId)
{
    const auto it = mDebugPathLines.find(spaceshipId);
    if (it != mDebugPathLines.end()) {
        for (auto& lineComp : it->second) {
            lineComp->SetIsVisible(false);
            lineComp->SetIsEnabled(false);
        }
        mDebugPathLines.erase(it);
    }
}

void NavigationController::RefreshAllDebugPaths()
{
    for (const auto& spaceship : mEnemies) {
        if (eSpaceshipActivityState::ACTIVE != spaceship->GetSpaceshipActivityState()) {
            continue;
        }
        const auto& routeMoveComp = spaceship->GetOnRouteMovementComponent();
        if (routeMoveComp) {
            CreateDebugPathForSpaceship(spaceship->GetObjectId(), routeMoveComp->GetRoutePoints());
        }
    }
}
#endif

} // namespace Game
