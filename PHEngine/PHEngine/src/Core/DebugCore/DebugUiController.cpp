#include "DebugUiController.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/Assertion.h"
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
              mInputComponent(std::make_unique<InputComponent>(std::make_shared<ComponentData>("DebugUiControllerInputComponent"))),
              mPressButtonCooldown(0.0f)
        {
            mInputComponent->SetIsReceivingMouseEvents(false);
        }

        DebugUiController::~DebugUiController()
        {
            WindowSizeChangedEvent::GetInstance()->RemoveListener(WindowSizeChangedEvent::GetInstanceId());
        }

        void DebugUiController::Initialize()
        {
            WindowSizeChangedEvent::GetInstance()->AddListener(shared_from_this());
        }

        void DebugUiController::SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp)
        {
            mSceneWp = sceneWp;
        }

        void DebugUiController::PostPlayLevelFinished()
        {
            if (!mCanvas)
            {
                InitializeWidgets();
            }
        }

        void DebugUiController::RecalculateWidgetsSize()
        {
            assert(mCanvas);
            assert(mRectangleBackground && mRenderThreadFrameRateLabel && mGameThreadFrameRateLabel &&
                   mLuaThreadFrameRateLabel && mImage && mImage1 && mNextPoolsArrowImage);

            const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
            const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();
            const auto imageMargin = 50;
            const auto imageHeight = (windowHeight / 2) - (4 * imageMargin);

            mRectangleBackground->SetWidth(imageHeight + (imageMargin * 2));
            mRectangleBackground->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
            mRectangleBackground->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mCanvas->GetName());
            mRectangleBackground->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mCanvas->GetName());

            mRenderThreadFrameRateLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
            mRenderThreadFrameRateLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
            mRenderThreadFrameRateLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mRectangleBackground->GetName());
            mRenderThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
            mRenderThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
            mRenderThreadFrameRateLabel->SetHeight(20);

            mGameThreadFrameRateLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
            mGameThreadFrameRateLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
            mGameThreadFrameRateLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mRenderThreadFrameRateLabel->GetName());
            mGameThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
            mGameThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
            mGameThreadFrameRateLabel->SetHeight(20);

            mLuaThreadFrameRateLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
            mLuaThreadFrameRateLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
            mLuaThreadFrameRateLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mGameThreadFrameRateLabel->GetName());
            mLuaThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
            mLuaThreadFrameRateLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
            mLuaThreadFrameRateLabel->SetHeight(20);

            mImage->SetHeight(imageHeight);
            mImage->SetWidth(imageHeight);
            mImage->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
            mImage->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mRectangleBackground->GetName());
            mImage->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
            mImage->SetAnchorMargin(eUiAnchor::LEFT, imageMargin);
            mImage->SetAnchorMargin(eUiAnchor::TOP, imageMargin * 3);
            mImage->SetAnchorMargin(eUiAnchor::RIGHT, imageMargin);

            mImage1->SetHeight(imageHeight);
            mImage1->SetWidth(imageHeight);
            mImage1->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
            mImage1->SetAnchor(eUiAnchor::TOP, eUiAnchor::BOTTOM, mImage->GetName());
            mImage1->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
            mImage1->SetAnchorMargin(eUiAnchor::LEFT, imageMargin);
            mImage1->SetAnchorMargin(eUiAnchor::TOP, imageMargin);
            mImage1->SetAnchorMargin(eUiAnchor::RIGHT, imageMargin);

            mNextPoolsArrowImage->SetHeight(imageHeight / 2);
            mNextPoolsArrowImage->SetWidth(imageHeight / 2);
            mNextPoolsArrowImage->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mRectangleBackground->GetName());
            mNextPoolsArrowImage->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
            mNextPoolsArrowImage->SetAnchorMargin(eUiAnchor::BOTTOM, 35);
            mNextPoolsArrowImage->SetAnchorMargin(eUiAnchor::LEFT, 35);
        }

        void DebugUiController::InitializeWidgets()
        {
            mPools.reserve(2);
            mPools.emplace_back(RenderTargetPool::GetInstance());
            mPools.emplace_back(TexturePool::GetInstance());

            if (const auto &sceneSp = mSceneWp.lock())
            {
                const auto windowWidth = DisplayDeviceDataProvider::GetInstance()->GetWindowWidth();
                const auto windowHeight = DisplayDeviceDataProvider::GetInstance()->GetWindowHeight();

                const auto &uiHandler = sceneSp->GetUiHandler();
                mCanvas = uiHandler->CreateDebugCanvas(ViewPortInfo(0, 0, windowWidth, windowHeight));
                mCanvas->InitializeInputSystem();
                mCanvas->SetIsVisible(false);
                mCanvas->SetZOrder(100000); // This canvas has to be the last in the render queue

                mRectangleBackground = std::make_shared<UiRectangle>();
                mRectangleBackground->SetParents(mCanvas, mCanvas);
                mRectangleBackground->SetColor(0xA5ABBE);
                mRectangleBackground->SetZOrder(1);
                mRectangleBackground->SetOpacity(0.0f);

                mRenderThreadFrameRateLabel = std::make_shared<UiLabel>("nimbus_mono");
                mRenderFpsLabel = mRenderThreadFrameRateLabel;
                mRenderThreadFrameRateLabel->SetParents(mCanvas, mRectangleBackground);
                mRenderThreadFrameRateLabel->SetTextColor(0xFF0000);
                mRenderThreadFrameRateLabel->SetFontSize(9.0f);
                mRenderThreadFrameRateLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
                mRenderThreadFrameRateLabel->SetZOrder(2);

                mGameThreadFrameRateLabel = std::make_shared<UiLabel>("nimbus_mono");
                mGameFpsLabel = mGameThreadFrameRateLabel;
                mGameThreadFrameRateLabel->SetParents(mCanvas, mRectangleBackground);
                mGameThreadFrameRateLabel->SetTextColor(0xFF0000);
                mGameThreadFrameRateLabel->SetFontSize(9.0f);
                mGameThreadFrameRateLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
                mGameThreadFrameRateLabel->SetZOrder(2);

                mLuaThreadFrameRateLabel = std::make_shared<UiLabel>("nimbus_mono");
                mLuaFpsLabel = mLuaThreadFrameRateLabel;
                mLuaThreadFrameRateLabel->SetParents(mCanvas, mRectangleBackground);
                mLuaThreadFrameRateLabel->SetTextColor(0xFF0000);
                mLuaThreadFrameRateLabel->SetFontSize(9.0f);
                mLuaThreadFrameRateLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
                mLuaThreadFrameRateLabel->SetZOrder(2);

                mImage = std::make_shared<UiImage>();
                mImage->SetParents(mCanvas, mRectangleBackground);
                mImage->SetOpacity(1);
                mImage->SetZOrder(2);

                mImage1 = std::make_shared<UiImage>();
                mImage1->SetParents(mCanvas, mRectangleBackground);
                mImage1->SetOpacity(1);
                mImage1->SetZOrder(2);

                mNextPoolsArrowImage = std::make_shared<UiImage>();
                mNextPoolsArrowImage->SetParents(mCanvas, mRectangleBackground);
                mNextPoolsArrowImage->SetOpacity(1);
                mNextPoolsArrowImage->SetZOrder(2);
                mNextPoolsArrowImage->SetTextureSrc("arrow_right_1.png");

                const auto &arrowMouseInputReceiver = std::make_shared<UiMouseInputReceiverBase>(mNextPoolsArrowImage);
                arrowMouseInputReceiver->SetMouseClickedCallback(std::bind(&DebugUiController::OnNextPoolButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
                mNextPoolsArrowImage->SetMouseInputReceiver(arrowMouseInputReceiver);

                mImages.emplace_back(mImage);
                mImages.emplace_back(mImage1);

                RecalculateWidgetsSize();
            }
        }

        void DebugUiController::Tick(const float deltaTime)
        {
        }

        void DebugUiController::UnpausableTick(const float deltaTime)
        {
            const auto &keyboardBindings = mInputComponent->GetKeyboardBindings();
            static constexpr float buttonCooldown = 0.5f;

            if (keyboardBindings->HasPressedKeys())
            {
                if (KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::Shift) && KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::P))
                {
                    mCanvas->SetIsVisible(false);
                    mPressButtonCooldown = 0.0f;
                }
                else if (KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::P))
                {
                    if (mPressButtonCooldown >= buttonCooldown)
                    {
                        mPressButtonCooldown = 0.0f;
                        if (!mCanvas->IsVisible())
                        {
                            mCanvas->SetIsVisible(true);
                        }
                        for (const auto &mImage : mImages)
                        {
                            mImage->SetTexture(GetNextTexture());
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

            for (const auto &mImage : mImages)
            {
                mImage->SetTexture(GetNextTexture());
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

        void DebugUiController::ProcessEvent(const WindowSizeChangedEvent::EventData_t &data)
        {
            RecalculateWidgetsSize();
        }
    }
}