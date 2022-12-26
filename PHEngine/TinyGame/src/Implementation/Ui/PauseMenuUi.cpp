#include "PauseMenuUi.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"

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
}