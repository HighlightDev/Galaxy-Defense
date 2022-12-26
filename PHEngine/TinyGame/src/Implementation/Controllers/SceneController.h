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

        std::unique_ptr<CombatController> mCombatController;

        std::unique_ptr<UiController> mUiController;

        std::shared_ptr<Actor> mAmbientMusicDummy;

    public:
        SceneController(const std::weak_ptr<::EngineCore::Scene> &scene);

        ~SceneController();

        virtual void OnPreLevelInit() override;

        virtual void OnLevelInit() override;

        virtual void OnPostLevelInit() override;

        virtual void PostPlayLevelFinished() override;

        virtual void Tick(const float deltaTime) override;

        virtual void UnpausableTick(const float deltaTime) override;

        void SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController);
    };
}