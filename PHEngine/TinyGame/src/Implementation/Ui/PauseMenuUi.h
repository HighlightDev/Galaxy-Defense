#pragma once

#include "Core/GameCore/GUI/OverlayManagement/IUiOverlay.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

#include <memory>

namespace EngineCore
{
    class Scene;
    namespace GUI
    {
        class OverlayManager;
    }
}

using namespace EngineCore::GUI;

namespace Game
{
    class PauseMenuUi : public IUiOverlay
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::weak_ptr<::EngineCore::GUI::OverlayManager> mOverlayManagerWp;

        std::shared_ptr<::EngineCore::GUI::UiCanvas> mPauseMenuCanvas;

        std::string mOverlayName;

    public:
        PauseMenuUi(const std::string &overlayName, const std::weak_ptr<::EngineCore::Scene> &sceneWp, const std::weak_ptr<::EngineCore::GUI::OverlayManager> &overlayManagerWp);

        void Initialize() override;

        void OpenOverlay() override;

        void CloseOverlay() override;

        std::string GetOverlayName() const override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        bool IsVisible() const;

        std::shared_ptr<::EngineCore::GUI::UiCanvas> GetCanvas() const override { return mPauseMenuCanvas; }

        void SubscribeOnAnimationFinished(const std::function<void(std::string)> &callback) override {}

        bool HasFadeInAnimation() const override { return false; }

        bool HasFadeOutAnimation() const override { return false; }

    private:
        void OnContinueButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnSettingsButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnExitToMainMenuButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnExitGameButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnButtonHoverEntered(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

        void OnButtonHoverLeaved(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);
    };
}