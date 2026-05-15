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
#include "Core/UtilityCore/EngineMath.h"
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

bool NavigationController::cEnableDebugPathRendering = false;

NavigationController::NavigationController(const std::weak_ptr<::EngineCore::Scene>& sceneWp)
    : mSceneWp(sceneWp)
    , mNavPathDummyActor(std::make_shared<Actor>(
          "NavPathDummyActor",
          std::make_shared<EngineCore::SceneComponent>(
              "NavPathDummy_rootComponent", glm::vec3(), glm::vec3(), glm::vec3(1), true)))
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
        auto* handler = spaceship->GetRouteHandler();
        if (handler && handler->UpdateRoutesAndCheckIfCompleted()) {
            handler->ResetState();
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
    return BuildNavMeshRouteTo(startPosition, mFinalDestinationPoint);
}

std::vector<glm::vec3>
NavigationController::BuildNavMeshRouteTo(const glm::vec3& startPosition, const glm::vec3& endPosition) const
{
    ext_assert(mNavMesh, "NavMesh is null in NavigationController::BuildNavMeshRoute");
    const glm::vec2 start2D(startPosition.x, startPosition.z);
    const glm::vec2 end2D(endPosition.x, endPosition.z);
    const auto route2D = mNavMesh->BuildRouteBetweenPoints(start2D, end2D);
    std::vector<glm::vec3> route3D;
    route3D.reserve(route2D.size());
    for (const auto& point : route2D) {
        route3D.emplace_back(point.x, startPosition.y, point.y);
    }

    // A* returns cell centers; replace the last point with the exact destination
    // so ships fly into the portal precisely rather than stopping at the cell center
    if (!route3D.empty()) {
        route3D.back() = glm::vec3(endPosition.x, startPosition.y, endPosition.z);
    }

    if (route3D.size() < 3) {
        return route3D;
    }

    // Smooth the path using quadratic Bezier segments.
    // Triplets (P[i], P[i+1]=control, P[i+2]) with stride 2; P[i+1] is the Bezier control point.
    static constexpr size_t c_bezierSubdivisions = 3;
    static constexpr float c_tStep = 1.0f / static_cast<float>(c_bezierSubdivisions + 1);

    std::vector<glm::vec3> smoothRoute;
    smoothRoute.reserve((route3D.size() / 2) * (c_bezierSubdivisions + 2));

    size_t i = 0;
    for (; i + 2 < route3D.size(); i += 2) {
        const auto& p0 = route3D[i];
        const auto& p1 = route3D[i + 1]; // Bezier control point
        const auto& p2 = route3D[i + 2]; // end of this segment / start of next
        smoothRoute.push_back(p0);
        for (size_t s = 1; s <= c_bezierSubdivisions; ++s) {
            const float t = static_cast<float>(s) * c_tStep;
            smoothRoute.push_back(EngineMath::QuadraticBezier(p0, p1, p2, t));
        }
        // p2 will be added as p0 of the next iteration, or by the trailing loop below
    }
    // Append any remaining points (covers the final endpoint and the even-count remainder)
    for (; i < route3D.size(); ++i) {
        smoothRoute.push_back(route3D[i]);
    }

    return smoothRoute;
}

std::vector<glm::vec3> NavigationController::BuildNavMeshRouteToNearestBarrier(const glm::vec3& startPosition) const
{
    std::vector<glm::vec3> bestRoute;

    for (const auto& barrierWp : mActiveBarriersOnLevel) {
        const auto barrier = barrierWp.lock();
        if (!barrier || eBarrierActivityState::ACTIVE != barrier->GetState()) {
            continue;
        }

        const auto pillars = barrier->GetBarrierPillarsMeshComponents();
        for (size_t pillarIndex = 0; pillarIndex < pillars.size(); ++pillarIndex) {
            if (!pillars[pillarIndex] || !pillars[pillarIndex]->IsEnabled()) {
                continue;
            }

            const auto pillarPosition = barrier->GetBarrierPillarPosition(static_cast<int32_t>(pillarIndex));
            const auto dirFromPillar = glm::normalize(startPosition - pillarPosition);
            const float cellSize = mNavMesh->GetCellSize();

            // Try cells approaching the pillar from our side: 1, 2, 3 cells back.
            // The pillar cell itself is non-walkable, so approach from the near side.
            for (int32_t offset = 1; offset <= 3; ++offset) {
                const auto candidate = pillarPosition + dirFromPillar * (cellSize * static_cast<float>(offset));
                if (!mNavMesh->IsCellWalkableByWorldPosition(glm::vec2(candidate.x, candidate.z))) {
                    continue;
                }
                const auto route = BuildNavMeshRouteTo(startPosition, candidate);
                if (!route.empty() && (bestRoute.empty() || route.size() < bestRoute.size())) {
                    bestRoute = route;
                }
                break;
            }
        }
    }

    return bestRoute;
}

void NavigationController::PutSpaceshipOnRoute(const glm::vec3& startPosition, const std::shared_ptr<SpaceshipActor>& spaceship)
{
    LogInfo(
        "NavigationController::PutSpaceshipOnRoute: putting spaceship ",
        spaceship->GetObjectId(),
        " on route from position: ",
        startPosition);
    auto* handler = spaceship->GetRouteHandler();
    ext_assert(handler, "SpaceshipRouteHandler is null in PutSpaceshipOnRoute");
    handler->SetRouteBuildFunctions(
        [weakMe = std::weak_ptr<NavigationController>(shared_from_this())](const glm::vec3& fromPosition) {
            if (const auto& sharedMe = weakMe.lock()) {
                return sharedMe->BuildNavMeshRoute(fromPosition);
            }
            return std::vector<glm::vec3>{};
        },
        [weakMe = std::weak_ptr<NavigationController>(shared_from_this())](const glm::vec3& fromPosition) {
            if (const auto& sharedMe = weakMe.lock()) {
                return sharedMe->BuildNavMeshRouteToNearestBarrier(fromPosition);
            }
            return std::vector<glm::vec3>{};
        });
    handler->InitializeAndStartFrom(startPosition);

    mEnemies.emplace_back(spaceship);
#ifdef DEBUG
    if (cEnableDebugPathRendering) {
        const auto route = handler->GetCurrentRoutePoints();
        if (route.size() > 1) {
            CreateDebugPathForSpaceship(spaceship->GetObjectId(), route);
        }
    }
#endif
}

void NavigationController::RebuildActiveShipRoutes()
{
    for (const auto& spaceship : mEnemies) {
        if (eSpaceshipActivityState::ACTIVE != spaceship->GetSpaceshipActivityState()) {
            continue;
        }
        auto* handler = spaceship->GetRouteHandler();
        if (!handler) {
            continue;
        }
        handler->RebuildFromCurrentPosition();
#ifdef DEBUG
        if (cEnableDebugPathRendering) {
            const auto route = handler->GetCurrentRoutePoints();
            if (route.size() > 1) {
                CreateDebugPathForSpaceship(spaceship->GetObjectId(), route);
            } else {
                RemoveDebugPathForSpaceship(spaceship->GetObjectId());
            }
        }
#endif
    }
}

void NavigationController::ReapplyAllObstaclesToNavMesh()
{
    mNavMesh->ResetAllCellsWalkable();

    for (const auto& barrierWp : mActiveBarriersOnLevel) {
        if (const auto barrier = barrierWp.lock()) {
            const auto& rayPositions = barrier->GetBarrierActiveRaysWorldPositions();
            for (const auto& [startPosition, endPosition] : rayPositions) {
                const auto& barrierPillarSize = barrier->GetBarrierPillarSize();
                mNavMesh->FillCellStatesBetweenWorldPositions(
                    glm::vec2(startPosition.x, startPosition.z),
                    glm::vec2(endPosition.x, endPosition.z),
                    glm::vec2(barrierPillarSize.x, barrierPillarSize.z),
                    false);
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
    const float cellSize = mNavMesh->GetCellSize();
    const auto& spaceStationSize
        = glm::vec2(spaceStationActor->GetSpaceStationSize().x, spaceStationActor->GetSpaceStationSize().z);

    const auto& maxCornerPos = center + (spaceStationSize * 0.5f);
    const auto& minCornerPos = center - (spaceStationSize * 0.5f);
    const int32_t occupiedCellsX = static_cast<int32_t>(std::ceil(spaceStationSize.x / cellSize));
    const int32_t occupiedCellsY = static_cast<int32_t>(std::ceil(spaceStationSize.y / cellSize));

    for (int32_t x = 0; x < occupiedCellsX; ++x) {
        for (int32_t y = 0; y < occupiedCellsY; ++y) {
            const auto& cellCenter = glm::vec2(
                minCornerPos.x + cellSize * (0.5f + static_cast<float>(x)),
                minCornerPos.y + cellSize * (0.5f + static_cast<float>(y)));
            mNavMesh->SetCellsStateByWorldPosition(cellCenter, glm::vec2(cellSize), isWalkable);
        }
    }
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
        const auto& barrierPillarSize = barrierActor->GetBarrierPillarSize();
        mNavMesh->FillCellStatesBetweenWorldPositions(
            glm::vec2(startPosition.x, startPosition.z),
            glm::vec2(endPosition.x, endPosition.z),
            glm::vec2(barrierPillarSize.x, barrierPillarSize.z),
            false);
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
        auto* handler = spaceship->GetRouteHandler();
        if (handler) {
            const auto route = handler->GetCurrentRoutePoints();
            if (route.size() > 1) {
                CreateDebugPathForSpaceship(spaceship->GetObjectId(), route);
            }
        }
    }
}
#endif

} // namespace Game
