#include "PlayerCombatUi.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverBase.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/GUI/UiElements/UiItem.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Event/PauseGameThreadEvent.h"
#include "Core/GameCore/Event/ExitGameThreadEvent.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;
using namespace IO;
using namespace Event;
using namespace EngineCore::GUI;

namespace Game
{
    PlayerCombatUi::PlayerCombatUi(const std::string &overlayName, const std::weak_ptr<Scene> &sceneWp, const std::weak_ptr<OverlayManager> &overlayManagerWp)
        : mSceneWp(sceneWp),
          mOverlayManagerWp(overlayManagerWp),
          mOverlayName(overlayName)
    {
    }

    void PlayerCombatUi::OpenOverlay()
    {
        if (!mCanvas->IsVisible())
        {
            mCanvas->SetIsVisible(true);
        }
    }

    void PlayerCombatUi::CloseOverlay()
    {
        if (mCanvas->IsVisible())
        {
            mCanvas->SetIsVisible(false);
        }
    }

    std::string PlayerCombatUi::GetOverlayName() const
    {
        return mOverlayName;
    }

    void PlayerCombatUi::Initialize()
    {
        if (const auto &sceneSp = mSceneWp.lock())
        {
            const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
            const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
            const auto &uiHandler = sceneSp->GetUiHandler();
            mCanvas = uiHandler->CreateCanvas(ViewPortInfo(0, 0, windowWidth, windowHeight));
            mCanvas->SetIsVisible(false);

            const auto rootContainerWidth = static_cast<int32_t>(static_cast<float>(windowWidth) / 3.3f);
            const auto rootContainerHeight = static_cast<int32_t>(static_cast<float>(windowHeight) / 4.0f);

            const auto &rootContainer = std::make_shared<UiItem>();
            rootContainer->SetParents(mCanvas, mCanvas);
            rootContainer->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
            rootContainer->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mCanvas->GetName());
            rootContainer->SetAnchorMargin(eUiAnchor::LEFT, 50);
            rootContainer->SetAnchorMargin(eUiAnchor::BOTTOM, 50);
            rootContainer->SetWidth(rootContainerWidth);
            rootContainer->SetHeight(rootContainerHeight);

            const auto &lifeImage = std::make_shared<UiImage>();
            lifeImage->SetParents(mCanvas, rootContainer);
            lifeImage->SetTextureSrc("combat_life1.png");
            lifeImage->SetRotationDegrees(180.0f);
            lifeImage->SetZOrder(2);
            lifeImage->SetHeight(rootContainerHeight / 2);
            lifeImage->SetWidth(rootContainerHeight / 2);
            lifeImage->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, rootContainer->GetName());
            lifeImage->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, rootContainer->GetName());
            lifeImage->SetAnchorMargin(eUiAnchor::LEFT, rootContainerHeight / 4);
            lifeImage->SetAnchorMargin(eUiAnchor::BOTTOM, rootContainerHeight / 4);
        }
    }

    void PlayerCombatUi::Tick(const float deltaTime)
    {
        mCanvas->Tick(deltaTime);
    }

    void PlayerCombatUi::UnpausableTick(const float deltaTime)
    {
        if (mCanvas)
        {
            mCanvas->UnpausableTick(deltaTime);
        }
    }

    bool PlayerCombatUi::IsVisible() const
    {
        return mCanvas ? mCanvas->IsVisible() : false;
    }
}