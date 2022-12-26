#pragma once

#include "ILevelController.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Ui/PauseMenuUi.h"

#include <memory>

namespace EngineCore
{
    class Scene;
    class InputComponent;
}

using namespace EngineCore::GUI;

namespace Game
{
    class UiController : public ITickable,
                         public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::unique_ptr<PauseMenuUi> mPauseMenu;

        std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

        float mPressButtonCooldown;

    public:
        UiController(const std::weak_ptr<::EngineCore::Scene> &scene);

        virtual void Tick(const float deltaTime) override;

        virtual void UnpausableTick(const float deltaTime) override;

        virtual void OnPreLevelInit() override;

        virtual void OnLevelInit() override;

        virtual void OnPostLevelInit() override;

        virtual void PostPlayLevelFinished() override;
    };
}