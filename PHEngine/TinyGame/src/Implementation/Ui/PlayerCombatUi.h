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
    class PlayerCombatUi : public IUiOverlay
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::weak_ptr<::EngineCore::GUI::OverlayManager> mOverlayManagerWp;

        std::shared_ptr<::EngineCore::GUI::UiCanvas> mCanvas;

        std::string mOverlayName;

    public:
        PlayerCombatUi(const std::string &overlayName, const std::weak_ptr<::EngineCore::Scene> &sceneWp, const std::weak_ptr<::EngineCore::GUI::OverlayManager> &overlayManagerWp);

        void Initialize() override;

        void OpenOverlay() override;

        void CloseOverlay() override;

        std::string GetOverlayName() const override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        bool IsVisible() const;
    };
}