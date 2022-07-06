#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Actor.h"

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

    class SceneController : public ITickable
    {
        std::weak_ptr<::EngineCore::Scene> mScene;
        
        std::unique_ptr<CombatController> mCombatController;

        std::shared_ptr<Actor> mAmbientMusicDummy;// todo: should smth better

    public:
        SceneController(const std::weak_ptr<::EngineCore::Scene> &scene);

        ~SceneController();

        virtual void PreInit();

        virtual void PostInit();

        virtual void PostPlayLevelFinished();

        virtual void Tick(const float deltaTime) override;

        void SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController);
    };
}