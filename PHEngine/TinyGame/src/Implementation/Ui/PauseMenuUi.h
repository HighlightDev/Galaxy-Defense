#pragma once

#include "IUiOverlay.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

#include <memory>

namespace EngineCore
{
    class Scene;
}

using namespace EngineCore::GUI;

namespace Game
{
    class OverlayManager;

    class PauseMenuUi : public IUiOverlay
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::weak_ptr<OverlayManager> mOverlayManagerWp;

        std::shared_ptr<::EngineCore::GUI::UiCanvas> mPauseMenuCanvas;

        std::string mOverlayName;

    public:
        PauseMenuUi(const std::string &overlayName, const std::weak_ptr<::EngineCore::Scene> &sceneWp, const std::weak_ptr<OverlayManager> &overlayManagerWp);

        void Initialize() override;

        void OpenOverlay() override;

        void CloseOverlay() override;

        std::string GetOverlayName() const override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        bool IsVisible() const;

    private:
        void OnContinueButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnExitToMainMenuButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnExitGameButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnButtonHoverEntered(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnButtonHoverLeaved(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);
    };
}