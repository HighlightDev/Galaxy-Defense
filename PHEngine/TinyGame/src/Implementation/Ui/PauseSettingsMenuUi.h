#pragma once

#include "Core/GameCore/GUI/OverlayManagement/IUiOverlay.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GameCore/GUI/UiElements/UiToggleButton.h"
#include "Core/GameCore/ITickable.h"

#include <memory>

namespace EngineCore {
class Scene;
namespace GUI {
class OverlayManager;
}
} // namespace EngineCore

using namespace EngineCore::GUI;

namespace Game {
class PauseSettingsMenuUi : public IUiOverlay {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::weak_ptr<OverlayManager> mOverlayManagerWp;

    std::shared_ptr<::EngineCore::GUI::UiCanvas> mPauseSettingsMenuCanvas;

    std::string mOverlayName;

public:
    PauseSettingsMenuUi(
        const std::string& overlayName,
        const std::weak_ptr<::EngineCore::Scene>& sceneWp,
        const std::weak_ptr<OverlayManager>& overlayManagerWp);

    void Initialize() override;

    void OpenOverlay() override;

    void CloseOverlay() override;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override;

    bool IsVisible() const;

    std::string GetOverlayName() const override;

    std::shared_ptr<::EngineCore::GUI::UiCanvas> GetCanvas() const override
    {
        return mPauseSettingsMenuCanvas;
    }

    void SubscribeOnAnimationFinished(const std::function<void(std::string)>& callback) override
    {
    }

    bool HasFadeInAnimation() const override
    {
        return false;
    }

    bool HasFadeOutAnimation() const override
    {
        return false;
    }

    void CleanUp() override;

private:
    void OnApplyButtonClicked(const std::weak_ptr<UiItemBase>& senderWp, const glm::ivec2& mouseCursorPosition);

    void OnCancelButtonClicked(const std::weak_ptr<UiItemBase>& senderWp, const glm::ivec2& mouseCursorPosition);

    void OnButtonHoverEntered(const std::weak_ptr<UiItemBase>& senderWp, const glm::ivec2& mouseCursorPosition);

    void OnButtonHoverLeaved(const std::weak_ptr<UiItemBase>& senderWp, const glm::ivec2& mouseCursorPosition);

    void OnSoundButtonToggled(const std::weak_ptr<UiToggleButton>& senderWp, const bool toggleButtonState);
};
} // namespace Game