#include "PauseMenuUi.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"
#include "Core/GameCore/Event/ExitGameThreadEvent.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiver.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;
using namespace IO;
using namespace Event;

namespace Game
{
    PauseMenuUi::PauseMenuUi(const std::weak_ptr<Scene> &sceneWp)
        : mSceneWp(sceneWp)
    {
    }

    void PauseMenuUi::ShowMenu()
    {
        if (!mPauseMenuCanvas->IsVisible())
        {
            PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, true);
            mPauseMenuCanvas->SetIsVisible(true);
        }
    }

    void PauseMenuUi::HideMenu()
    {
        if (mPauseMenuCanvas->IsVisible())
        {
            PauseGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, false);
            mPauseMenuCanvas->SetIsVisible(false);
        }
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

            const auto menuHorizontalMargin = windowWidth / 3;
            const auto menuVerticalMargin = windowHeight / 7;

            const auto &backgroundRect = std::make_shared<UiRectangle>(mPauseMenuCanvas, mPauseMenuCanvas);
            mPauseMenuCanvas->AddUiItem(backgroundRect);
            backgroundRect->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mPauseMenuCanvas->GetName());
            backgroundRect->SetAnchorMargin(eUiAnchor::LEFT, menuHorizontalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::RIGHT, menuHorizontalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::BOTTOM, menuVerticalMargin);
            backgroundRect->SetAnchorMargin(eUiAnchor::TOP, menuVerticalMargin);
            backgroundRect->SetColor(0x6C5B7BFF);
            backgroundRect->SetZOrder(1);

            const auto buttonColor = 0x403649FF;
            const auto pauseMenuHeight = (windowHeight - (menuVerticalMargin * 2));
            constexpr auto buttonsCount = 3;
            constexpr auto buttonsMarginCount = buttonsCount + 1;
            auto buttonHeight = (pauseMenuHeight / buttonsCount);
            const auto buttonVerticalMarginHeight = buttonHeight / 3;
            const auto totalButtonMarginHeight = buttonsMarginCount * buttonVerticalMarginHeight;
            buttonHeight = (pauseMenuHeight - totalButtonMarginHeight) / 3;
            
            const auto &continueButton = std::make_shared<UiRectangle>(mPauseMenuCanvas, backgroundRect);
            backgroundRect->AddUiItem(continueButton);
            continueButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            continueButton->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, backgroundRect->GetName());
            continueButton->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, backgroundRect->GetName());
            continueButton->SetAnchorMargin(eUiAnchor::LEFT, 20);
            continueButton->SetAnchorMargin(eUiAnchor::RIGHT, 20);
            continueButton->SetAnchorMargin(eUiAnchor::TOP, buttonVerticalMarginHeight);
            continueButton->SetHeight(buttonHeight);
            continueButton->SetColor(buttonColor);
            continueButton->SetZOrder(2);

            const auto &exitToMainMenuButton = std::make_shared<UiRectangle>(mPauseMenuCanvas, backgroundRect);
            backgroundRect->AddUiItem(exitToMainMenuButton);
            exitToMainMenuButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            exitToMainMenuButton->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, backgroundRect->GetName());
            exitToMainMenuButton->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, continueButton->GetName());
            exitToMainMenuButton->SetAnchorMargin(eUiAnchor::LEFT, 20);
            exitToMainMenuButton->SetAnchorMargin(eUiAnchor::RIGHT, 20);
            exitToMainMenuButton->SetAnchorMargin(eUiAnchor::TOP, buttonVerticalMarginHeight);
            exitToMainMenuButton->SetHeight(buttonHeight);
            exitToMainMenuButton->SetColor(buttonColor);
            exitToMainMenuButton->SetZOrder(2);

            const auto &exitGameButton = std::make_shared<UiRectangle>(mPauseMenuCanvas, backgroundRect);
            backgroundRect->AddUiItem(exitGameButton);
            exitGameButton->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, backgroundRect->GetName());
            exitGameButton->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, backgroundRect->GetName());
            exitGameButton->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, exitToMainMenuButton->GetName());
            exitGameButton->SetAnchorMargin(eUiAnchor::LEFT, 20);
            exitGameButton->SetAnchorMargin(eUiAnchor::RIGHT, 20);
            exitGameButton->SetAnchorMargin(eUiAnchor::TOP, buttonVerticalMarginHeight);
            exitGameButton->SetHeight(buttonHeight);
            exitGameButton->SetColor(buttonColor);
            exitGameButton->SetZOrder(2);

            const auto& continueButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiver>();
            continueButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseMenuUi::OnContinueButtonClicked, this, std::placeholders::_1));

            const auto& exitToMainMenuButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiver>();
            exitToMainMenuButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseMenuUi::OnExitToMainMenuButtonClicked, this, std::placeholders::_1));

            const auto& exitGameButtonMouseInputReceiver = std::make_shared<UiMouseInputReceiver>();
            exitGameButtonMouseInputReceiver->SetMouseClickedCallback(std::bind(&PauseMenuUi::OnExitGameButtonClicked, this, std::placeholders::_1));

            continueButton->SetMouseInputReceiver(continueButtonMouseInputReceiver);
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

    void PauseMenuUi::OnContinueButtonClicked(const glm::ivec2& mouseCursorPosition)
    {
        HideMenu();
    }

    void PauseMenuUi::OnExitToMainMenuButtonClicked(const glm::ivec2& mouseCursorPosition)
    {
        LogInfo("PauseMenuUi::OnExitToMainMenuButtonClicked => Not implemented yet.");
    }

    void PauseMenuUi::OnExitGameButtonClicked(const glm::ivec2& mouseCursorPosition)
    {
        Event::ExitGameThreadEvent::GetInstance()->SendEvent(Event::eExecutionOrder::POST_EXECUTION);
    }
}