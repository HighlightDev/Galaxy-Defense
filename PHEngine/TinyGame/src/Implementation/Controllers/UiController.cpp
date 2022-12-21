#include "UiController.h"

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"

using namespace IO;

namespace Game
{
    UiController::UiController(const std::weak_ptr<Scene> &scene)
        : mSceneWp(scene),
          mCanvas()
    {
    }

    void UiController::Tick(const float deltaTime)
    {
    }

    void UiController::OnPreLevelInit()
    {
    }

    void UiController::OnLevelInit()
    {
    }

    void UiController::OnPostLevelInit()
    {
    }

    void UiController::PostPlayLevelFinished()
    {
        /*if (const auto &sceneSp = mSceneWp.lock())
        {
            const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
            const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
            const auto halfWidth = windowWidth / 2;
            const auto halfHeight = windowHeight / 2;
            const auto originX = halfWidth - (halfWidth / 2);
            const auto originY = halfHeight - (halfHeight / 2);
            const auto &uiHandler = sceneSp->GetUiHandler();
            mCanvas = uiHandler->CreateCanvas(ViewPortInfo(originX, originY, halfWidth, halfHeight));
            const auto &uiImage = std::make_shared<UiImage>(mCanvas, mCanvas);
            uiImage->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
            uiImage->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mCanvas->GetName());
            uiImage->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mCanvas->GetName());
            uiImage->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mCanvas->GetName());
            uiImage->SetAnchorMargin(eUiAnchor::LEFT, 0);
            uiImage->SetAnchorMargin(eUiAnchor::BOTTOM, 0);
            uiImage->SetTextureSrc("path.png");
            uiImage->SetOpacity(1);
            uiImage->SetZOrder(1);

            const auto &uiImage1 = std::make_shared<UiImage>(mCanvas, uiImage);
            uiImage1->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, uiImage->GetName());
            uiImage1->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, uiImage->GetName());
            uiImage1->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, uiImage->GetName());
            uiImage1->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, uiImage->GetName());

            uiImage1->SetAnchorMargin(eUiAnchor::LEFT, 100);
            uiImage1->SetAnchorMargin(eUiAnchor::RIGHT, 50);
            uiImage1->SetAnchorMargin(eUiAnchor::TOP, 50);
            uiImage1->SetAnchorMargin(eUiAnchor::BOTTOM, 100);
            uiImage1->SetTextureSrc("grass.png");
            uiImage1->SetZOrder(2);

            const auto &uiImage2 = std::make_shared<UiImage>(mCanvas, uiImage1);
            uiImage2->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, uiImage1->GetName());
            uiImage2->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, uiImage1->GetName());
            uiImage2->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, uiImage1->GetName());
            uiImage2->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, uiImage1->GetName());
            uiImage2->SetAnchorMargin(eUiAnchor::LEFT, 50);
            uiImage2->SetAnchorMargin(eUiAnchor::RIGHT, 50);
            uiImage2->SetAnchorMargin(eUiAnchor::TOP, 50);
            uiImage2->SetAnchorMargin(eUiAnchor::BOTTOM, 50);
            uiImage2->SetTextureSrc("path.png");
            uiImage2->SetZOrder(3);

            mCanvas->AddUiItem(uiImage);
            uiImage->AddUiItem(uiImage1);
            uiImage1->AddUiItem(uiImage2);
        }*/
    }
}