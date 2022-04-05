#pragma once

#include "Core/GameCore/ITickable.h"

#include <memory>

namespace EngineCore
{
    class Scene;
};

namespace Game
{
    class SpaceShipPlayerController;
    class CombatController;

    class SceneController : public ITickable
    {
        std::unique_ptr<CombatController> mCombatController;

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