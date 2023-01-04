#pragma once

#include "ILevelController.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Ui/OverlayManager.h"

#include <memory>

namespace EngineCore
{
    class Scene;
    class InputComponent;
}

namespace Game
{
    class UiController : public ITickable,
                         public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::shared_ptr<OverlayManager> mOverlayManager;

        std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

        float mPressButtonCooldown;

    public:
        UiController(const std::weak_ptr<::EngineCore::Scene> &scene);

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;
    };
}