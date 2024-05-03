#pragma once

#include "ILevelController.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Implementation/Navigation/NavigationPathBuilder.h"
#include "Implementation/DamageDealerType.h"

#include <memory>
#include <vector>

namespace EngineCore
{
    class Scene;
    class Actor;
}

using namespace EngineCore;

namespace Game
{
    class SpaceshipActor;
    class MissileActor;

    class NavigationController
        : public ITickable,
          public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        NavigationPathBuilder mNavPathBuilder;

        std::vector<std::shared_ptr<SpaceshipActor>> mEnemies;

        std::vector<std::shared_ptr<MissileActor>> mMissiles;

        std::shared_ptr<::EngineCore::Actor> mNavPathDummyActor;

        BoundingBox3D mLevelBounds;

    public:
        explicit NavigationController(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

        void SetPathRoutes(const std::unordered_map<std::string, Path> &paths);

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void CleanUp() override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        std::vector<std::string> GetPathNames() const;

        void SetLevelBounds(const BoundingBox3D &levelBounds);

        void PutSpaceshipOnRoute(const std::string &routeName, const std::shared_ptr<SpaceshipActor> &spaceship);

        void PutMissileToNavigate(const std::shared_ptr<MissileActor> &missile);

        void RemoveSpaceshipFromRoute(const int32_t spaceshipActorId);

        void RemoveMissileFromNavigation(const int32_t missileActorId);

    private:
        void Initialize();

        void InitializePathDebugRendering();
    };
}
