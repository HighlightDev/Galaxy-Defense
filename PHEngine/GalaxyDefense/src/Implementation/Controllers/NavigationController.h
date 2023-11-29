#pragma once

#include "ILevelController.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Navigation/NavigationPathBuilder.h"

#include <memory>
#include <vector>

namespace EngineCore
{
    class Scene;
    class Actor;
}

namespace Game
{
    class SpaceshipActor;

    class NavigationController
        : public ITickable,
          public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        NavigationPathBuilder mNavPathBuilder;

        std::vector<std::shared_ptr<SpaceshipActor>> mEnemies;

        std::shared_ptr<::EngineCore::Actor> mNavPathDummyActor;

    public:
        explicit NavigationController(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void CleanUp() override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void SetEnemies(const std::vector<std::shared_ptr<SpaceshipActor>> &enemies);

    private:
        void Initialize();
    };
}
