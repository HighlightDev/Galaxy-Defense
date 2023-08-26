#pragma once

#include "UiController.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Actor.h"
#include "ILevelController.h"

#include <memory>

namespace EngineCore
{
    class Scene;
    class Actor;
};

namespace Game
{
    class SpaceShipPlayerController;
    class CombatController;

    class SceneController : public ITickable,
                            public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mScene;

        std::shared_ptr<CombatController> mCombatController;

        std::unique_ptr<UiController> mUiController;

        std::shared_ptr<Actor> mAmbientMusicDummy;

    public:
        SceneController(const std::weak_ptr<::EngineCore::Scene> &scene);

        ~SceneController();

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController);

        void CleanUp() override;
    };
}