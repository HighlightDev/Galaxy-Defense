#include "DebugUiController.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverBase.h"

using namespace EngineCore;
using namespace IO;
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
            mPools.reserve(2);
            mPools.emplace_back(RenderTargetPool::GetInstance());
            mPools.emplace_back(TexturePool::GetInstance());

            if (const auto &sceneSp = mSceneWp.lock())
            {
                const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
                const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
                const auto imageMargin = 50;
                const auto imageHeight = (windowHeight / 2) - (4 * imageMargin);

                const auto &uiHandler = sceneSp->GetUiHandler();
                mCanvas = uiHandler->CreateDebugCanvas(ViewPortInfo(0, 0, windowWidth, windowHeight));
                mCanvas->InitializeInputSystem();
                mCanvas->SetIsVisible(false);

                const auto &rectangleBackground = std::make_shared<UiRectangle>();
                rectangleBackground->SetParents(mCanvas, mCanvas);
                rectangleBackground->SetColor(0xA5ABBE);
                rectangleBackground->SetWidth(imageHeight + (imageMargin * 2));
                rectangleBackground->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
                rectangleBackground->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mCanvas->GetName());
                rectangleBackground->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mCanvas->GetName());
                rectangleBackground->SetZOrder(1);
                rectangleBackground->SetOpacity(0.3f);

                const auto &renderThreadFrameRateLabel = std::make_shared<UiLabel>("nimbus_mono");
                mRenderFpsLabel = renderThreadFrameRateLabel;
                renderThreadFrameRateLabel->SetParents(mCanvas, rectangleBackground);
                renderThreadFrameRateLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, rectangleBackground->GetName());
                renderThreadFrameRateLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, rectangleBackground->GetName());
                renderThreadFrameRateLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, rectangleBackground->GetName());
                renderThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
                renderThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
                renderThreadFrameRateLabel->SetHeight(20);
                renderThreadFrameRateLabel->SetTextColor(0xD68E97);
                renderThreadFrameRateLabel->SetFontSize(9.0f);
                renderThreadFrameRateLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
                renderThreadFrameRateLabel->SetZOrder(2);

                const auto &gameThreadFrameRateLabel = std::make_shared<UiLabel>("nimbus_mono");
                mGameFpsLabel = gameThreadFrameRateLabel;
                gameThreadFrameRateLabel->SetParents(mCanvas, rectangleBackground);
                gameThreadFrameRateLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, rectangleBackground->GetName());
                gameThreadFrameRateLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, rectangleBackground->GetName());
                gameThreadFrameRateLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, renderThreadFrameRateLabel->GetName());
                gameThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
                gameThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
                gameThreadFrameRateLabel->SetHeight(20);
                gameThreadFrameRateLabel->SetTextColor(0xD68E97);
                gameThreadFrameRateLabel->SetFontSize(9.0f);
                gameThreadFrameRateLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
                gameThreadFrameRateLabel->SetZOrder(2);

                const auto &luaThreadFrameRateLabel = std::make_shared<UiLabel>("nimbus_mono");
                mLuaFpsLabel = luaThreadFrameRateLabel;
                luaThreadFrameRateLabel->SetParents(mCanvas, rectangleBackground);
                luaThreadFrameRateLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, rectangleBackground->GetName());
                luaThreadFrameRateLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, rectangleBackground->GetName());
                luaThreadFrameRateLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, gameThreadFrameRateLabel->GetName());
                luaThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
                luaThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
                luaThreadFrameRateLabel->SetHeight(20);
                luaThreadFrameRateLabel->SetTextColor(0xD68E97);
                luaThreadFrameRateLabel->SetFontSize(9.0f);
                luaThreadFrameRateLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
                luaThreadFrameRateLabel->SetZOrder(2);

                const auto &image = std::make_shared<UiImage>();
                image->SetParents(mCanvas, rectangleBackground);
                image->SetHeight(imageHeight);
                image->SetWidth(imageHeight);
                image->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, rectangleBackground->GetName());
                image->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, rectangleBackground->GetName());
                image->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, rectangleBackground->GetName());
                image->SetAnchorMargin(eUiAnchor::LEFT, imageMargin);
                image->SetAnchorMargin(eUiAnchor::TOP, imageMargin * 3);
                image->SetAnchorMargin(eUiAnchor::RIGHT, imageMargin);
                image->SetOpacity(1);
                image->SetZOrder(2);

                const auto &image1 = std::make_shared<UiImage>();
                image1->SetParents(mCanvas, rectangleBackground);
                image1->SetHeight(imageHeight);
                image1->SetWidth(imageHeight);
                image1->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, rectangleBackground->GetName());
                image1->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, image->GetName());
                image1->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, rectangleBackground->GetName());
                image1->SetAnchorMargin(eUiAnchor::LEFT, imageMargin);
                image1->SetAnchorMargin(eUiAnchor::TOP, imageMargin);
                image1->SetAnchorMargin(eUiAnchor::RIGHT, imageMargin);
                image1->SetOpacity(1);
                image1->SetZOrder(2);

                const auto &nextPoolsArrowImage = std::make_shared<UiImage>();
                nextPoolsArrowImage->SetParents(mCanvas, rectangleBackground);
                nextPoolsArrowImage->SetHeight(imageHeight / 2);
                nextPoolsArrowImage->SetWidth(imageHeight / 2);
                nextPoolsArrowImage->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, rectangleBackground->GetName());
                nextPoolsArrowImage->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, rectangleBackground->GetName());
                nextPoolsArrowImage->SetAnchorMargin(eUiAnchor::BOTTOM, 35);
                nextPoolsArrowImage->SetAnchorMargin(eUiAnchor::LEFT, 35);
                nextPoolsArrowImage->SetOpacity(1);
                nextPoolsArrowImage->SetZOrder(2);
                nextPoolsArrowImage->SetTextureSrc("arrow_right_1.png");

                const auto &arrowMouseInputReceiver = std::make_shared<UiMouseInputReceiverBase>(nextPoolsArrowImage);
                arrowMouseInputReceiver->SetMouseClickedCallback(std::bind(&DebugUiController::OnNextPoolButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
                nextPoolsArrowImage->SetMouseInputReceiver(arrowMouseInputReceiver);

                mImages.emplace_back(image);
                mImages.emplace_back(image1);
            }
        }

        void DebugUiController::Tick(const float deltaTime)
        {
        }

        void DebugUiController::UnpausableTick(const float deltaTime)
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
                            image->SetTexture(GetNextTexture());
                        }
                    }
                }
            }

            mPressButtonCooldown += deltaTime;
        }

        std::shared_ptr<ITexture> DebugUiController::GetNextTexture() const
        {
            const auto &activePool = mPools[mPoolIndex];
            const auto totalCount = activePool->GetTexturesCount();
            mTextureIndex = mTextureIndex > (totalCount - 1) ? 0 : mTextureIndex;

            return activePool->GetTextureAt(mTextureIndex++);
        }

        void DebugUiController::OnNextPoolButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition)
        {
            mTextureIndex = 0;
            if ((++mPoolIndex) >= mPools.size())
            {
                mPoolIndex = 0;
            }

            for (const auto &image : mImages)
            {
                image->SetTexture(GetNextTexture());
            }
        }

        void DebugUiController::SetRenderFpsText(const std::string &fpsText)
        {
            if (mRenderFpsLabel)
            {
                mRenderFpsLabel->SetText("Render FPS: " + fpsText);
            }
        }

        void DebugUiController::SetGameFpsText(const std::string &fpsText)
        {
            if (mGameFpsLabel)
            {
                mGameFpsLabel->SetText("Game FPS: " + fpsText);
            }
        }

        void DebugUiController::SetLuaFpsText(const std::string &fpsText)
        {
            if (mLuaFpsLabel)
            {
                mLuaFpsLabel->SetText("Lua FPS: " + fpsText);
            }
        }
    }
}