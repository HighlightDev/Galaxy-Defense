#include "PauseSettingsMenuUi.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"
#include "Core/GameCore/Event/ExitGameThreadEvent.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiver.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/LoggerExtension.h"
#include "OverlayManager.h"

using namespace EngineCore;
using namespace IO;
using namespace Event;

namespace Game
{
    static constexpr uint32_t s_buttonColor = 0x403649FF;
    static constexpr uint32_t s_hoveredButtonColor = 0x201b24FF;

    PauseSettingsMenuUi::PauseSettingsMenuUi(const std::string &overlayName, const std::weak_ptr<::EngineCore::Scene> &sceneWp, const std::weak_ptr<OverlayManager> &overlayManagerWp)
        : mSceneWp(sceneWp),
          mOverlayManagerWp(overlayManagerWp),
          mOverlayName(overlayName)
    {
    }

    void PauseSettingsMenuUi::OpenOverlay()
    {
        if (!mPauseSettingsMenuCanvas->IsVisible())
        {
            PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, true);
            mPauseSettingsMenuCanvas->SetIsVisible(true);
        }
    }

    void PauseSettingsMenuUi::CloseOverlay()
    {
        if (mPauseSettingsMenuCanvas->IsVisible())
        {
            PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, false);
            mPauseSettingsMenuCanvas->SetIsVisible(false);
        }
    }

    void PauseSettingsMenuUi::Initialize()
    {
        if (const auto &sceneSp = mSceneWp.lock())
        {
            const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
            const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
            const auto &uiHandler = sceneSp->GetUiHandler();
            mPauseSettingsMenuCanvas = uiHandler->CreateCanvas(ViewPortInfo(0, 0, windowWidth, windowHeight));
            mPauseSettingsMenuCanvas->InitializeInputSystem();
            mPauseSettingsMenuCanvas->SetIsVisible(false);

            const auto menuHorizontalMargin = static_cast<int32_t>(static_cast<float>(windowWidth) / 4.0f);
            const auto menuVerticalMargin = windowHeight / 7;

            const auto &backgroundRect = std::make_shared<UiRectangle>(mPauseSettingsMenuCanvas, mPauseSettingsMenuCanvas);
            mPauseSettingsMenuCanvas->AddUiItem(backgroundRect);
            backgroundRect->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mPauseSettingsMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mPauseSettingsMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mPauseSettingsMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mPauseSettingsMenuCanvas->GetName());
            backgroundRect->SetAnchorMargin(eUiAnchor::LEFT, menuHorizontalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::RIGHT, menuHorizontalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::BOTTOM, menuVerticalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::TOP, menuVerticalMargin);
            backgroundRect->SetColor(0x6C5B7BFF);
            backgroundRect->SetZOrder(1);
        }
    }

    void PauseSettingsMenuUi::Tick(const float deltaTime)
    {
        if (mPauseSettingsMenuCanvas)
        {
            mPauseSettingsMenuCanvas->Tick(deltaTime);
        }
    }

    void PauseSettingsMenuUi::UnpausableTick(const float deltaTime)
    {
        if (mPauseSettingsMenuCanvas)
        {
            mPauseSettingsMenuCanvas->UnpausableTick(deltaTime);
        }
    }

    bool PauseSettingsMenuUi::IsVisible() const
    {
        return mPauseSettingsMenuCanvas ? mPauseSettingsMenuCanvas->IsVisible() : false;
    }

    std::string PauseSettingsMenuUi::GetOverlayName() const
    {
        return mOverlayName;
    }
}