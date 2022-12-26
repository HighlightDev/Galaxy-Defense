#pragma once

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
    class PauseMenuUi
        : public ITickable
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::shared_ptr<::EngineCore::GUI::UiCanvas> mPauseMenuCanvas;

    public:
        PauseMenuUi(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

        void Initialize();

        void ShowMenu();

        void HideMenu();

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        bool IsVisible() const;

    private:
        void OnContinueButtonClicked(const glm::ivec2 &mouseCursorPosition);

        void OnExitToMainMenuButtonClicked(const glm::ivec2 &mouseCursorPosition);

        void OnExitGameButtonClicked(const glm::ivec2 &mouseCursorPosition);
    };
}