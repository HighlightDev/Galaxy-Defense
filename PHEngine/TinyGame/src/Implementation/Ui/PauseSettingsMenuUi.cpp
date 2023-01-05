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
            mPauseSettingsMenuCanvas->SetIsVisible(true);
        }
    }

    void PauseSettingsMenuUi::CloseOverlay()
    {
        if (mPauseSettingsMenuCanvas->IsVisible())
        {
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

            const auto rowButtonsCount = 2;
            const auto backgroundRectWidth = windowWidth - (menuHorizontalMargin * 2);
            const auto &buttonHorizontalMargin = static_cast<int32_t>(static_cast<float>(backgroundRectWidth) / 10.0f);
            const auto &buttonWidth = (backgroundRectWidth - (buttonHorizontalMargin * (rowButtonsCount + 1))) / rowButtonsCount;

            const auto &applyButton = std::make_shared<UiRectangle>(mPauseSettingsMenuCanvas, backgroundRect);
            backgroundRect->AddUiItem(applyButton);
            applyButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            applyButton->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, backgroundRect->GetName());
            applyButton->SetAnchorMargin(eUiAnchor::LEFT, buttonHorizontalMargin);
            applyButton->SetAnchorMargin(eUiAnchor::BOTTOM, 50);
            applyButton->SetWidth(buttonWidth);
            applyButton->SetHeight(100);
            applyButton->SetColor(s_buttonColor);
            applyButton->SetZOrder(2);

            const auto &applyButtonLabel = std::make_shared<UiLabel>(mPauseSettingsMenuCanvas, applyButton, "nimbus_mono");
            applyButton->AddUiItem(applyButtonLabel);
            applyButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, applyButton->GetName());
            applyButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, applyButton->GetName());
            applyButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, applyButton->GetName());
            applyButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, applyButton->GetName());
            applyButtonLabel->SetText("Apply");
            applyButtonLabel->SetTextColor(0xFFFFFF);
            applyButtonLabel->SetFontSize(20.0f);
            applyButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            applyButtonLabel->SetZOrder(3);

            const auto &cancelButton = std::make_shared<UiRectangle>(mPauseSettingsMenuCanvas, backgroundRect);
            backgroundRect->AddUiItem(cancelButton);
            cancelButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::RIGHT, applyButton->GetName());
            cancelButton->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, backgroundRect->GetName());
            cancelButton->SetAnchorMargin(eUiAnchor::LEFT, buttonHorizontalMargin);
            cancelButton->SetAnchorMargin(eUiAnchor::BOTTOM, 50);
            cancelButton->SetWidth(buttonWidth);
            cancelButton->SetHeight(100);
            cancelButton->SetColor(s_buttonColor);
            cancelButton->SetZOrder(2);

            const auto &cancelButtonLabel = std::make_shared<UiLabel>(mPauseSettingsMenuCanvas, cancelButton, "nimbus_mono");
            cancelButton->AddUiItem(cancelButtonLabel);
            cancelButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, cancelButton->GetName());
            cancelButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, cancelButton->GetName());
            cancelButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, cancelButton->GetName());
            cancelButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, cancelButton->GetName());
            cancelButtonLabel->SetText("Cancel");
            cancelButtonLabel->SetTextColor(0xFFFFFF);
            cancelButtonLabel->SetFontSize(20.0f);
            cancelButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            cancelButtonLabel->SetZOrder(3);

            const auto &applyButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiver>(applyButton);
            applyButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseSettingsMenuUi::OnApplyButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
            applyButtonMouseInputReceiver->SetMouseHoverEnteredCallback(std::bind(&PauseSettingsMenuUi::OnButtonHoverEntered, this, std::placeholders::_1, std::placeholders::_2));
            applyButtonMouseInputReceiver->SetMouseHoverLeavedCallback(std::bind(&PauseSettingsMenuUi::OnButtonHoverLeaved, this, std::placeholders::_1, std::placeholders::_2));

            const auto &cancelButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiver>(cancelButton);
            cancelButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseSettingsMenuUi::OnCancelButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
            cancelButtonMouseInputReceiver->SetMouseHoverEnteredCallback(std::bind(&PauseSettingsMenuUi::OnButtonHoverEntered, this, std::placeholders::_1, std::placeholders::_2));
            cancelButtonMouseInputReceiver->SetMouseHoverLeavedCallback(std::bind(&PauseSettingsMenuUi::OnButtonHoverLeaved, this, std::placeholders::_1, std::placeholders::_2));

            applyButton->SetMouseInputReceiver(applyButtonMouseInputReceiver);
            cancelButton->SetMouseInputReceiver(cancelButtonMouseInputReceiver);
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

    void PauseSettingsMenuUi::OnApplyButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        if (const auto& overlayManagerSp = mOverlayManagerWp.lock())
        {
            overlayManagerSp->OpenOverlay("PauseMenu");
        }
    }

    void PauseSettingsMenuUi::OnCancelButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        if (const auto& overlayManagerSp = mOverlayManagerWp.lock())
        {
            overlayManagerSp->OpenOverlay("PauseMenu");
        }
    }

    void PauseSettingsMenuUi::OnButtonHoverEntered(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        if (const auto &senderSp = senderWp.lock())
        {
            const auto &rectangleSp = std::static_pointer_cast<UiRectangle>(senderSp);
            assert(rectangleSp);
            rectangleSp->SetColor(s_hoveredButtonColor);
        }
    }

    void PauseSettingsMenuUi::OnButtonHoverLeaved(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {

        if (const auto &senderSp = senderWp.lock())
        {
            const auto &rectangleSp = std::static_pointer_cast<UiRectangle>(senderSp);
            assert(rectangleSp);
            rectangleSp->SetColor(s_buttonColor);
        }
    }
}