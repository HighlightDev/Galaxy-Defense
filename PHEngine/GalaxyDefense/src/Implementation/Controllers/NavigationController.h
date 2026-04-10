#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/NavigationMesh/NavMesh2D.h"
#include "ILevelController.h"
#include "Implementation/Controllers/BarriersController.h"
#include "Implementation/DamageDealerType.h"
#include "Implementation/Navigation/NavigationPathBuilder.h"

#include <memory>
#include <vector>

namespace EngineCore {
class Scene;
class Actor;
class StaticMeshComponent;
} // namespace EngineCore

using namespace EngineCore;

namespace Game {
class SpaceshipActor;
class MissileActor;
class BarrierActor;

class NavigationController : public ITickable, public ILevelController {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    NavigationPathBuilder mNavPathBuilder;

    std::vector<std::shared_ptr<SpaceshipActor>> mEnemies;

    std::vector<std::shared_ptr<MissileActor>> mMissiles;

    std::shared_ptr<::EngineCore::Actor> mNavPathDummyActor;

    BoundingBox3D mLevelBounds;

    static constexpr bool cEnableDebugPathRendering{false};

    BarriersController mBarriersController;

    std::unique_ptr<EngineCore::NavigationMesh::NavMesh2D> mNavMesh;

    std::vector<std::weak_ptr<BarrierActor>> mActiveBarriersOnLevel;

#ifdef DEBUG
    std::shared_ptr<::EngineCore::Actor> mNavMeshDebugActor;
    std::vector<std::vector<std::weak_ptr<::EngineCore::StaticMeshComponent>>> mNavMeshDebugGreenCells;
    std::vector<std::vector<std::weak_ptr<::EngineCore::StaticMeshComponent>>> mNavMeshDebugRedCells;
#endif

public:
    explicit NavigationController(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    void SetPathRoutes(const std::unordered_map<std::string, Path>& paths);

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void CleanUp() override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    std::vector<std::string> GetPathNames() const;

    const Path& GetPath(const std::string& pathName) const;

    const std::unordered_multimap<std::string, std::pair<std::string, Path>>& GetExtendedPaths() const;

    void SetLevelBounds(const BoundingBox3D& levelBounds);

    void PutSpaceshipOnRoute(const std::string& routeName, const std::shared_ptr<SpaceshipActor>& spaceship);

    void PutMissileToNavigate(const std::shared_ptr<MissileActor>& missile);

    void PutActiveBarrierOnLevel(const std::shared_ptr<BarrierActor>& barrierActor);

    void RemoveActiveBarrierFromLevel(const std::shared_ptr<BarrierActor>& barrierActor);

    void RemoveSpaceshipFromRoute(const int32_t spaceshipActorId);

    void RemoveMissileFromNavigation(const int32_t missileActorId);

    BarriersController& GetBarriersController();

    const BarriersController& GetBarriersController() const;

private:
    void Initialize();

    void InitializePathDebugRendering();

    void InitializeNavMesh();

#ifdef DEBUG
    void InitializeNavMeshDebugRendering();

    void RefreshNavMeshDebugRendering();
#endif
};
} // namespace Game
