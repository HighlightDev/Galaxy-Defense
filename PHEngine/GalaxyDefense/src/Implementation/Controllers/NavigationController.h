#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/ITickable.h"
#include "ILevelController.h"
#include "Implementation/DamageDealerType.h"
#include "Implementation/Navigation/NavigationPathBuilder.h"

#include <memory>
#include <vector>

namespace EngineCore {
class Scene;
class Actor;
} // namespace EngineCore

using namespace EngineCore;

namespace Game {
class SpaceshipActor;
class MissileActor;

class NavigationController : public ITickable, public ILevelController {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    NavigationPathBuilder mNavPathBuilder;

    std::vector<std::shared_ptr<SpaceshipActor>> mEnemies;

    std::vector<std::shared_ptr<MissileActor>> mMissiles;

    std::shared_ptr<::EngineCore::Actor> mNavPathDummyActor;

    BoundingBox3D mLevelBounds;

    static constexpr bool cEnableDebugPathRendering{false};

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

    void RemoveSpaceshipFromRoute(const int32_t spaceshipActorId);

    void RemoveMissileFromNavigation(const int32_t missileActorId);

private:
    void Initialize();

    void InitializePathDebugRendering();
};
} // namespace Game
