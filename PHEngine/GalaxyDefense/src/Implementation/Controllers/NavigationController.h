#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/NavigationMesh/NavMesh2D.h"
#include "ILevelController.h"
#include "Implementation/Actors/RouteHandlers/SpaceshipRouteHandler.h"
#include "Implementation/DamageDealerType.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace Graphics {
class IMaterial;
}

namespace EngineCore {
class Scene;
class Actor;
class StaticMeshComponent;
class RuntimeGeneratedLineComponent;
} // namespace EngineCore

using namespace EngineCore;

namespace Game {
class SpaceshipActor;
class MissileActor;
class BarrierActor;
class SpaceStationActor;

class NavigationController : public ITickable,
                             public ILevelController,
                             public std::enable_shared_from_this<NavigationController> {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::vector<std::shared_ptr<SpaceshipActor>> mEnemies;

    std::vector<std::shared_ptr<MissileActor>> mMissiles;

    std::shared_ptr<::EngineCore::Actor> mNavPathDummyActor;

    BoundingBox3D mLevelBounds;

    std::unique_ptr<EngineCore::NavigationMesh::NavMesh2D> mNavMesh;

    std::vector<std::weak_ptr<BarrierActor>> mActiveBarriersOnLevel;

    std::vector<std::weak_ptr<SpaceStationActor>> mActiveSpaceStationsOnLevel;

    glm::vec3 mFinalDestinationPoint;

    std::vector<glm::vec3> mPortalPositions;

#ifdef DEBUG
    std::shared_ptr<::EngineCore::Actor> mNavMeshDebugActor;
    std::vector<std::vector<std::weak_ptr<::EngineCore::StaticMeshComponent>>> mNavMeshDebugGreenCells;
    std::vector<std::vector<std::weak_ptr<::EngineCore::StaticMeshComponent>>> mNavMeshDebugRedCells;

    static bool cEnableDebugPathRendering;
    std::shared_ptr<::EngineCore::Actor> mDebugPathActor;
    std::shared_ptr<Graphics::IMaterial> mDebugPathMaterial;
    std::unordered_map<int32_t, std::vector<std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent>>> mDebugPathLines;
#endif

public:
    explicit NavigationController(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    void SetFinalDestinationPoint(const glm::vec3& destinationPoint);

    void SetPortalPositions(const std::vector<glm::vec3>& portalPositions);

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void CleanUp() override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    void SetLevelBounds(const BoundingBox3D& levelBounds);

    void PutSpaceshipOnRoute(const glm::vec3& startPosition, const std::shared_ptr<SpaceshipActor>& spaceship);

    void PutMissileToNavigate(const std::shared_ptr<MissileActor>& missile);

    void PutActiveBarrierOnLevel(const std::shared_ptr<BarrierActor>& barrierActor);

    void RemoveActiveBarrierFromLevel(const std::shared_ptr<BarrierActor>& barrierActor);

    void PutActiveSpaceStationOnLevel(const std::shared_ptr<SpaceStationActor>& spaceStationActor);

    void RemoveActiveSpaceStationFromLevel(const std::shared_ptr<SpaceStationActor>& spaceStationActor);

    void RemoveSpaceshipFromRoute(const int32_t spaceshipActorId);

    void RemoveMissileFromNavigation(const int32_t missileActorId);

    bool IsPositionNearFinalDestination(const glm::vec3& position, const float radius) const;

private:
    void Initialize();

    void InitializeNavMesh();

    std::vector<glm::vec3> BuildNavMeshRoute(const glm::vec3& startPosition) const;

    std::vector<glm::vec3> BuildNavMeshRouteTo(const glm::vec3& startPosition, const glm::vec3& endPosition) const;

    std::vector<glm::vec3> BuildNavMeshRouteToNearestBarrier(const glm::vec3& startPosition) const;

    void ReapplyAllObstaclesToNavMesh();

    void MarkSpaceStationCellsOnNavMesh(const std::shared_ptr<SpaceStationActor>& spaceStationActor, const bool isWalkable);

    void RebuildActiveShipRoutes();

#ifdef DEBUG
    void InitializeNavMeshDebugRendering();

    void RefreshNavMeshDebugRendering();

    void InitializeDebugPathRendering();

    void CreateDebugPathForSpaceship(const int32_t spaceshipId, const std::vector<glm::vec3>& routePoints);

    void RemoveDebugPathForSpaceship(const int32_t spaceshipId);

    void RefreshAllDebugPaths();
#endif
};
} // namespace Game
