#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "IUiOverlay.h"

#include <memory>

namespace EngineCore
{
    class Scene;
}

using namespace EngineCore::GUI;

namespace Game
{
    class OverlayManager;

    class PauseSettingsMenuUi
        : public IUiOverlay
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::weak_ptr<OverlayManager> mOverlayManagerWp;

        std::shared_ptr<::EngineCore::GUI::UiCanvas> mPauseSettingsMenuCanvas;

        std::string mOverlayName;

    public:
        PauseSettingsMenuUi(const std::string &overlayName, const std::weak_ptr<::EngineCore::Scene> &sceneWp, const std::weak_ptr<OverlayManager> &overlayManagerWp);

        void Initialize() override;

        void OpenOverlay() override;

        void CloseOverlay() override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        bool IsVisible() const;

        std::string GetOverlayName() const override;
    };
}