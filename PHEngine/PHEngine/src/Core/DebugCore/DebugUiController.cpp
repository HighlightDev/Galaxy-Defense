#include "DebugUiController.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"

using namespace EngineCore;
using namespace IO;
using namespace Resources;
using namespace Graphics;

namespace EngineCore
{
    namespace Debug
    {
        DebugUiController::DebugUiController()
            : mSceneWp(),
              mCanvas(),
              mInputComponent(std::make_unique<InputComponent>(ComponentData("DebugUiControllerInputComponent"))),
              mPressButtonCooldown(0.0f)
        {
            mInputComponent->SetIsReceivingMouseEvents(false);
        }

        void DebugUiController::SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp)
        {
            mSceneWp = sceneWp;
        }

        void DebugUiController::PostPlayLevelFinished()
        {
            Init();
        }

        void DebugUiController::Init()
        {
            if (const auto &sceneSp = mSceneWp.lock())
            {
                const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
                const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
                const auto imageMargin = 50;
                const auto imageHeight = (windowHeight / 2) - (2 * imageMargin);

                const auto &uiHandler = sceneSp->GetUiHandler();
                mCanvas = uiHandler->CreateCanvas(ViewPortInfo(0, 0, windowWidth, windowHeight));
                mCanvas->InitializeInputSystem();
                mCanvas->SetIsVisible(false);
                const auto &image = std::make_shared<UiImage>(mCanvas, mCanvas);
                mCanvas->AddUiItem(image);
                image->SetHeight(imageHeight);
                image->SetWidth(imageHeight);
                image->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
                image->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mCanvas->GetName());
                image->SetAnchorMargin(eUiAnchor::LEFT, imageMargin);
                image->SetAnchorMargin(eUiAnchor::TOP, imageMargin);
                image->SetOpacity(1);
                image->SetZOrder(1);

                const auto &image1 = std::make_shared<UiImage>(mCanvas, mCanvas);
                mCanvas->AddUiItem(image1);
                image1->SetHeight(imageHeight);
                image1->SetWidth(imageHeight);
                image1->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
                image1->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, image->GetName());
                image1->SetAnchorMargin(eUiAnchor::LEFT, imageMargin);
                image1->SetAnchorMargin(eUiAnchor::TOP, imageMargin);
                image1->SetOpacity(1);
                image1->SetZOrder(1);

                mImages.emplace_back(image);
                mImages.emplace_back(image1);
            }
        }

        void DebugUiController::Tick(const float deltaTime)
        {
            const auto &keyboardBindings = mInputComponent->GetKeyboardBindings();
            static constexpr float buttonCooldown = 0.5f;

            if (keyboardBindings.HasPressedKeys())
            {
                if (KeyState::PRESSED == keyboardBindings.GetStateByKey(eKeyboardKeys::Shift) && KeyState::PRESSED == keyboardBindings.GetStateByKey(eKeyboardKeys::P))
                {
                    mCanvas->SetIsVisible(false);
                    mPressButtonCooldown = 0.0f;
                }
                else if (KeyState::PRESSED == keyboardBindings.GetStateByKey(eKeyboardKeys::P))
                {
                    if (mPressButtonCooldown >= buttonCooldown)
                    {
                        mPressButtonCooldown = 0.0f;
                        if (!mCanvas->IsVisible())
                        {
                            mCanvas->SetIsVisible(true);
                        }
                        for (const auto &image : mImages)
                        {
                            image->SetTexture(GetNextRenderTargetTexture());
                        }
                    }
                }
            }

            mPressButtonCooldown += deltaTime;
        }

        std::shared_ptr<ITexture> DebugUiController::GetNextRenderTargetTexture() const
        {
            const size_t totalCount = RenderTargetPool::GetInstance()->GetResourcesCount();
            mRenderTargetIndex = mRenderTargetIndex > (totalCount - 1) ? 0 : mRenderTargetIndex;

            return RenderTargetPool::GetInstance()->GetRenderTargetAt(mRenderTargetIndex++);
        }
    }
}