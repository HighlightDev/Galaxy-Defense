#include "PauseMenuUi.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"
#include "Core/GameCore/Event/ExitGameThreadEvent.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverBase.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"

using namespace EngineCore;
using namespace IO;
using namespace Event;
using namespace EngineCore::GUI;

namespace Game
{
    static constexpr uint32_t s_buttonColor = 0x403649;
    static constexpr uint32_t s_hoveredButtonColor = 0x201b24;

    PauseMenuUi::PauseMenuUi(const std::string& overlayName, const std::weak_ptr<Scene> &sceneWp, const std::weak_ptr<OverlayManager>& overlayManagerWp)
        : mSceneWp(sceneWp),
          mOverlayManagerWp(overlayManagerWp),
          mOverlayName(overlayName)
    {
    }

    void PauseMenuUi::OpenOverlay()
    {
        if (!mPauseMenuCanvas->IsVisible())
        {
            mPauseMenuCanvas->SetIsVisible(true);
        }
    }

    void PauseMenuUi::CloseOverlay()
    {
        if (mPauseMenuCanvas->IsVisible())
        {
            mPauseMenuCanvas->SetIsVisible(false);
        }
    }

    std::string PauseMenuUi::GetOverlayName() const
    {
        return mOverlayName;
    }

    void PauseMenuUi::Initialize()
    {
        if (const auto &sceneSp = mSceneWp.lock())
        {
            const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
            const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
            const auto &uiHandler = sceneSp->GetUiHandler();
            mPauseMenuCanvas = uiHandler->CreateCanvas(ViewPortInfo(0, 0, windowWidth, windowHeight));
            mPauseMenuCanvas->InitializeInputSystem();
            mPauseMenuCanvas->SetIsVisible(false);

            const auto menuHorizontalMargin = static_cast<int32_t>(static_cast<float>(windowWidth) / 4.0f);
            const auto menuVerticalMargin = windowHeight / 15;

            const auto &backgroundRect = std::make_shared<UiRectangle>();
            backgroundRect->SetParents(mPauseMenuCanvas, mPauseMenuCanvas);
            backgroundRect->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchorMargin(eUiAnchor::LEFT, menuHorizontalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::RIGHT, menuHorizontalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::BOTTOM, menuVerticalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::TOP, menuVerticalMargin);
            backgroundRect->SetColor(0x6C5B7B);
            backgroundRect->SetZOrder(1);

            const auto pauseMenuHeight = (windowHeight - (menuVerticalMargin * 2));
            constexpr auto buttonsCount = 4;
            constexpr auto buttonsMarginCount = buttonsCount + 1;
            auto buttonHeight = (pauseMenuHeight / buttonsCount);
            const auto buttonVerticalMarginHeight = buttonHeight / 4;
            const auto totalButtonMarginHeight = buttonsMarginCount * buttonVerticalMarginHeight;
            buttonHeight = (pauseMenuHeight - totalButtonMarginHeight) / buttonsCount;

            const auto &continueButton = std::make_shared<UiRectangle>();
            continueButton->SetParents(mPauseMenuCanvas, backgroundRect);
            continueButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            continueButton->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, backgroundRect->GetName());
            continueButton->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, backgroundRect->GetName());
            continueButton->SetAnchorMargin(eUiAnchor::LEFT, 20);
            continueButton->SetAnchorMargin(eUiAnchor::RIGHT, 20);
            continueButton->SetAnchorMargin(eUiAnchor::TOP, buttonVerticalMarginHeight);
            continueButton->SetHeight(buttonHeight);
            continueButton->SetColor(s_buttonColor);
            continueButton->SetZOrder(2);

            const auto &continueButtonLabel = std::make_shared<UiLabel>("nimbus_mono");
            continueButtonLabel->SetParents(mPauseMenuCanvas, continueButton);
            continueButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, continueButton->GetName());
            continueButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, continueButton->GetName());
            continueButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, continueButton->GetName());
            continueButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, continueButton->GetName());
            continueButtonLabel->SetText("Continue");
            continueButtonLabel->SetTextColor(0xFFFFFF);
            continueButtonLabel->SetFontSize(20.0f);
            continueButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            continueButtonLabel->SetZOrder(3);

            const auto &settingsButton = std::make_shared<UiRectangle>();
            settingsButton->SetParents(mPauseMenuCanvas, backgroundRect);
            settingsButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            settingsButton->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, backgroundRect->GetName());
            settingsButton->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, continueButton->GetName());
            settingsButton->SetAnchorMargin(eUiAnchor::LEFT, 20);
            settingsButton->SetAnchorMargin(eUiAnchor::RIGHT, 20);
            settingsButton->SetAnchorMargin(eUiAnchor::TOP, buttonVerticalMarginHeight);
            settingsButton->SetHeight(buttonHeight);
            settingsButton->SetColor(s_buttonColor);
            settingsButton->SetZOrder(2);

            const auto &settingsButtonLabel = std::make_shared<UiLabel>("nimbus_mono");
            settingsButtonLabel->SetParents(mPauseMenuCanvas, settingsButton);
            settingsButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, settingsButton->GetName());
            settingsButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, settingsButton->GetName());
            settingsButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, settingsButton->GetName());
            settingsButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, settingsButton->GetName());
            settingsButtonLabel->SetText("Settings");
            settingsButtonLabel->SetTextColor(0xFFFFFF);
            settingsButtonLabel->SetFontSize(20.0f);
            settingsButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            settingsButtonLabel->SetZOrder(3);

            const auto &exitToMainMenuButton = std::make_shared<UiRectangle>();
            exitToMainMenuButton->SetParents(mPauseMenuCanvas, backgroundRect);
            exitToMainMenuButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            exitToMainMenuButton->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, backgroundRect->GetName());
            exitToMainMenuButton->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, settingsButton->GetName());
            exitToMainMenuButton->SetAnchorMargin(eUiAnchor::LEFT, 20);
            exitToMainMenuButton->SetAnchorMargin(eUiAnchor::RIGHT, 20);
            exitToMainMenuButton->SetAnchorMargin(eUiAnchor::TOP, buttonVerticalMarginHeight);
            exitToMainMenuButton->SetHeight(buttonHeight);
            exitToMainMenuButton->SetColor(s_buttonColor);
            exitToMainMenuButton->SetZOrder(2);

            const auto &exitToMainMenuButtonLabel = std::make_shared<UiLabel>("nimbus_mono");
            exitToMainMenuButtonLabel->SetParents(mPauseMenuCanvas, exitToMainMenuButton);
            exitToMainMenuButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, exitToMainMenuButton->GetName());
            exitToMainMenuButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, exitToMainMenuButton->GetName());
            exitToMainMenuButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, exitToMainMenuButton->GetName());
            exitToMainMenuButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, exitToMainMenuButton->GetName());
            exitToMainMenuButtonLabel->SetText("Exit to main menu");
            exitToMainMenuButtonLabel->SetTextColor(0xFFFFFF);
            exitToMainMenuButtonLabel->SetFontSize(20.0f);
            exitToMainMenuButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            exitToMainMenuButtonLabel->SetZOrder(3);

            const auto &exitGameButton = std::make_shared<UiRectangle>();
            exitGameButton->SetParents(mPauseMenuCanvas, backgroundRect);
            exitGameButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            exitGameButton->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, backgroundRect->GetName());
            exitGameButton->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, exitToMainMenuButton->GetName());
            exitGameButton->SetAnchorMargin(eUiAnchor::LEFT, 20);
            exitGameButton->SetAnchorMargin(eUiAnchor::RIGHT, 20);
            exitGameButton->SetAnchorMargin(eUiAnchor::TOP, buttonVerticalMarginHeight);
            exitGameButton->SetHeight(buttonHeight);
            exitGameButton->SetColor(s_buttonColor);
            exitGameButton->SetZOrder(2);

            const auto &exitGameMenuButtonLabel = std::make_shared<UiLabel>("nimbus_mono");
            exitGameMenuButtonLabel->SetParents(mPauseMenuCanvas, exitGameButton);
            exitGameMenuButtonLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, exitGameButton->GetName());
            exitGameMenuButtonLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, exitGameButton->GetName());
            exitGameMenuButtonLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, exitGameButton->GetName());
            exitGameMenuButtonLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, exitGameButton->GetName());
            exitGameMenuButtonLabel->SetText("Exit game");
            exitGameMenuButtonLabel->SetTextColor(0xFFFFFF);
            exitGameMenuButtonLabel->SetFontSize(20.0f);
            exitGameMenuButtonLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::CENTER);
            exitGameMenuButtonLabel->SetZOrder(3);

            const auto &continueButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiverBase>(continueButton);
            continueButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseMenuUi::OnContinueButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
            continueButtonMouseInputReceiver->SetMouseHoverEnteredCallback(std::bind(&PauseMenuUi::OnButtonHoverEntered, this, std::placeholders::_1, std::placeholders::_2));
            continueButtonMouseInputReceiver->SetMouseHoverLeavedCallback(std::bind(&PauseMenuUi::OnButtonHoverLeaved, this, std::placeholders::_1, std::placeholders::_2));

            const auto &settingsButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiverBase>(settingsButton);
            settingsButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseMenuUi::OnSettingsButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
            settingsButtonMouseInputReceiver->SetMouseHoverEnteredCallback(std::bind(&PauseMenuUi::OnButtonHoverEntered, this, std::placeholders::_1, std::placeholders::_2));
            settingsButtonMouseInputReceiver->SetMouseHoverLeavedCallback(std::bind(&PauseMenuUi::OnButtonHoverLeaved, this, std::placeholders::_1, std::placeholders::_2));

            const auto &exitToMainMenuButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiverBase>(exitToMainMenuButton);
            exitToMainMenuButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseMenuUi::OnExitToMainMenuButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
            exitToMainMenuButtonMouseInputReceiver->SetMouseHoverEnteredCallback(std::bind(&PauseMenuUi::OnButtonHoverEntered, this, std::placeholders::_1, std::placeholders::_2));
            exitToMainMenuButtonMouseInputReceiver->SetMouseHoverLeavedCallback(std::bind(&PauseMenuUi::OnButtonHoverLeaved, this, std::placeholders::_1, std::placeholders::_2));

            const auto &exitGameButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiverBase>(exitGameButton);
            exitGameButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseMenuUi::OnExitGameButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
            exitGameButtonMouseInputReceiver->SetMouseHoverEnteredCallback(std::bind(&PauseMenuUi::OnButtonHoverEntered, this, std::placeholders::_1, std::placeholders::_2));
            exitGameButtonMouseInputReceiver->SetMouseHoverLeavedCallback(std::bind(&PauseMenuUi::OnButtonHoverLeaved, this, std::placeholders::_1, std::placeholders::_2));

            continueButton->SetMouseInputReceiver(continueButtonMouseInputReceiver);
            settingsButton->SetMouseInputReceiver(settingsButtonMouseInputReceiver);
            exitToMainMenuButton->SetMouseInputReceiver(exitToMainMenuButtonMouseInputReceiver);
            exitGameButton->SetMouseInputReceiver(exitGameButtonMouseInputReceiver);
        }
    }

    void PauseMenuUi::Tick(const float deltaTime)
    {
        mPauseMenuCanvas->Tick(deltaTime);
    }

    void PauseMenuUi::UnpausableTick(const float deltaTime)
    {
        if (mPauseMenuCanvas)
        {
            mPauseMenuCanvas->UnpausableTick(deltaTime);
        }
    }

    bool PauseMenuUi::IsVisible() const
    {
        return mPauseMenuCanvas ? mPauseMenuCanvas->IsVisible() : false;
    }

    void PauseMenuUi::OnContinueButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        if (const auto& overlayManagerSp = mOverlayManagerWp.lock())
        {
            overlayManagerSp->CloseCurrentOverlay();
            PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, false);
        }
    }

    void PauseMenuUi::OnSettingsButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        if (const auto& overlayManagerSp = mOverlayManagerWp.lock())
        {
            overlayManagerSp->OpenOverlay("PauseSettingsMenu");
        }
    }

    void PauseMenuUi::OnExitToMainMenuButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        LogInfo("PauseMenuUi::OnExitToMainMenuButtonClicked => Not implemented yet.");
    }

    void PauseMenuUi::OnExitGameButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        Event::ExitGameThreadEvent::GetInstance()->SendEvent(Event::eExecutionOrder::POST_EXECUTION);
    }

    void PauseMenuUi::OnButtonHoverEntered(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        if (const auto &senderSp = senderWp.lock())
        {
            const auto &rectangleSp = std::static_pointer_cast<UiRectangle>(senderSp);
            assert(rectangleSp);
            rectangleSp->SetColor(s_hoveredButtonColor);
        }
    }

    void PauseMenuUi::OnButtonHoverLeaved(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
    {
        if (const auto &senderSp = senderWp.lock())
        {
            const auto &rectangleSp = std::static_pointer_cast<UiRectangle>(senderSp);
            assert(rectangleSp);
            rectangleSp->SetColor(s_buttonColor);
        }
    }
}