#pragma once

#include "ILevelController.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"

#include <memory>

namespace EngineCore
{
    class Scene;
}

using namespace EngineCore::GUI;

namespace Game
{
    class UiController : public ITickable,
                         public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::shared_ptr<::EngineCore::GUI::UiCanvas> mCanvas;

    public:
        UiController(const std::weak_ptr<::EngineCore::Scene> &scene);

        virtual void Tick(const float deltaTime) override;

        virtual void OnPreLevelInit() override;

        virtual void OnLevelInit() override;

        virtual void OnPostLevelInit() override;

        virtual void PostPlayLevelFinished() override;
    };
}