#include "DebugUiController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/UiInputComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiRowLayout.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverBase.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Debug {
DebugUiController::DebugUiController()
    : mSceneWp()
    , mCanvas()
    , mInputComponent(std::make_unique<UiInputComponent>(std::make_shared<ComponentData>("DebugUiControllerInputComponent")))
    , mPressButtonCooldown(0.0f)
{
    mInputComponent->SetIsReceivingMouseEvents(false);
}

DebugUiController::~DebugUiController()
{
    WindowSizeChangedGameThreadEvent::GetInstance()->RemoveListener(WindowSizeChangedGameThreadEvent::GetInstanceId());
}

void DebugUiController::Initialize()
{
    WindowSizeChangedGameThreadEvent::GetInstance()->AddListener(shared_from_this());
}

void DebugUiController::SetScene(const std::weak_ptr<::EngineCore::Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void DebugUiController::PostPlayLevelFinished()
{
    if (!mCanvas) {
        InitializeWidgets();
    }
}

void DebugUiController::RecalculateWidgetsSize()
{
    ext_assert(mCanvas, "DebugUiController canvas is null in RecalculateWidgetsSize");
    ext_assert(
        mRectangleBackground && mRenderFpsLabel && mGameFpsLabel && mLuaFpsLabel && mImage1 && mImage2 && mNextPoolsArrowImage,
        "One or more UI elements are null in DebugUiController::RecalculateWidgetsSize");

    const auto windowWidth = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth();
    const auto windowHeight = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight();
    const auto imageMargin = 50;
    const auto imageHeight = (windowHeight / 2) - (4 * imageMargin);
    const auto imageWidth = (windowWidth / 2.5) - (4 * imageMargin);

    mRectangleBackground->SetWidth(imageWidth + (imageMargin * 2));
    mRectangleBackground->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
    mRectangleBackground->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mCanvas->GetName());
    mRectangleBackground->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mCanvas->GetName());

    mTexturesLayout->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mCanvas->GetName());
    mTexturesLayout->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mCanvas->GetName());
    mTexturesLayout->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mCanvas->GetName());
    mTexturesLayout->SetHeight(imageWidth + (imageMargin * 2));
    mTexturesLayout->SetAlignment(eUiRowAlignmentType::CENTER);
    mTexturesLayout->SetSpacing(imageMargin);

    mImage1Container->SetHeight(imageWidth * 1.25);
    mImage1Container->SetWidth(imageWidth * 1.25);

    mImage2Container->SetHeight(imageWidth * 1.25);
    mImage2Container->SetWidth(imageWidth * 1.25);

    mImage1Label->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mImage1Container->GetName());
    mImage1Label->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mImage1Container->GetName());
    mImage1Label->SetAnchorMargin(eUiAnchor::TOP, 10);
    mImage1Label->SetHeight(imageWidth / 2);
    mImage1Label->SetWidth(imageWidth);

    mImage2Label->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mImage2Container->GetName());
    mImage2Label->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mImage2Container->GetName());
    mImage2Label->SetAnchorMargin(eUiAnchor::TOP, 10);
    mImage2Label->SetHeight(imageWidth / 2);
    mImage2Label->SetWidth(imageWidth);

    mRenderFpsLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
    mRenderFpsLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
    mRenderFpsLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mRectangleBackground->GetName());
    mRenderFpsLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
    mRenderFpsLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
    mRenderFpsLabel->SetHeight(30);

    mGameFpsLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
    mGameFpsLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
    mGameFpsLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mRenderFpsLabel->GetName());
    mGameFpsLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
    mGameFpsLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
    mGameFpsLabel->SetHeight(30);

    mLuaFpsLabel->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mRectangleBackground->GetName());
    mLuaFpsLabel->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mRectangleBackground->GetName());
    mLuaFpsLabel->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mGameFpsLabel->GetName());
    mLuaFpsLabel->SetAnchorMargin(eUiAnchor::LEFT, 20);
    mLuaFpsLabel->SetAnchorMargin(eUiAnchor::TOP, 30);
    mLuaFpsLabel->SetHeight(30);

    mImage1->SetHeight(imageWidth);
    mImage1->SetWidth(imageWidth);
    mImage1->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mImage1Container->GetName());
    mImage1->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mImage1Container->GetName());
    mImage2->SetHeight(imageWidth);
    mImage2->SetWidth(imageWidth);
    mImage2->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mImage2Container->GetName());
    mImage2->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mImage2Container->GetName());
    mNextPoolsArrowImage->SetHeight(imageWidth / 2);
    mNextPoolsArrowImage->SetWidth(imageWidth / 2);
}

void DebugUiController::InitializeWidgets()
{
    mPools.reserve(2);
    mPools.emplace_back(RenderTargetPool::GetInstance());
    mPools.emplace_back(TexturePool::GetInstance());

    if (const auto& sceneSp = mSceneWp.lock()) {
        const auto windowWidth = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth();
        const auto windowHeight = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight();

        const auto& uiHandler = sceneSp->GetUiHandler();
        mCanvas = uiHandler->CreateDebugCanvas(ViewPortInfo(0, 0, windowWidth, windowHeight));
        mCanvas->InitializeInputSystem();
        mCanvas->SetIsVisible(false);
        mCanvas->SetZOrder(100000); // This canvas has to be the last in the render queue

        mRectangleBackground = std::make_shared<UiRectangle>("DebugPanelContainer");
        mRectangleBackground->Initialize();
        mRectangleBackground->SetParents(mCanvas, mCanvas);
        mRectangleBackground->SetColor(0xA5ABBE);
        mRectangleBackground->SetZOrder(1);
        mRectangleBackground->SetOpacity(0.0f);

        mRenderFpsLabel = std::make_shared<UiLabel>("JetBrainsMono-VariableFont_wght", "DebugPanelRenderFPSLabel");
        mRenderFpsLabel->Initialize();
        mRenderFpsLabel->SetParents(mCanvas, mRectangleBackground);
        mRenderFpsLabel->SetTextColor(0xFF0000);
        mRenderFpsLabel->SetFontSize(16);
        mRenderFpsLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
        mRenderFpsLabel->SetTextVerticalAlignment(eTextVerticalAlignmentType::TOP);
        mRenderFpsLabel->SetZOrder(2);

        mGameFpsLabel = std::make_shared<UiLabel>("JetBrainsMono-VariableFont_wght", "DebugPanelGameFPSLabel");
        mGameFpsLabel->Initialize();
        mGameFpsLabel->SetParents(mCanvas, mRectangleBackground);
        mGameFpsLabel->SetTextColor(0xFF0000);
        mGameFpsLabel->SetFontSize(16);
        mGameFpsLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
        mGameFpsLabel->SetTextVerticalAlignment(eTextVerticalAlignmentType::TOP);
        mGameFpsLabel->SetZOrder(2);

        mLuaFpsLabel = std::make_shared<UiLabel>("JetBrainsMono-VariableFont_wght", "DebugPanelLuaFPSLabel");
        mLuaFpsLabel->Initialize();
        mLuaFpsLabel->SetParents(mCanvas, mRectangleBackground);
        mLuaFpsLabel->SetTextColor(0xFF0000);
        mLuaFpsLabel->SetFontSize(16);
        mLuaFpsLabel->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
        mLuaFpsLabel->SetTextVerticalAlignment(eTextVerticalAlignmentType::TOP);
        mLuaFpsLabel->SetZOrder(2);

        mTexturesLayout = std::make_shared<UiRowLayout>("TexturesLayout");
        mTexturesLayout->Initialize();
        mTexturesLayout->SetParents(mCanvas, mCanvas);

        mImage1Container = std::make_shared<UiItem>("Image1Container");
        mImage1Container->SetParents(mCanvas, mTexturesLayout);

        mImage2Container = std::make_shared<UiItem>("Image2Container");
        mImage2Container->SetParents(mCanvas, mTexturesLayout);

        mImage1Label = std::make_shared<UiLabel>("JetBrainsMono-VariableFont_wght", "DebugImage1Label");
        mImage1Label->Initialize();
        mImage1Label->SetParents(mCanvas, mImage1Container);
        mImage1Label->SetTextColor(0xFFFFFF);
        mImage1Label->SetFontSize(15);
        mImage1Label->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
        mImage1Label->SetTextVerticalAlignment(eTextVerticalAlignmentType::TOP);
        mImage1Label->SetZOrder(2);

        mImage2Label = std::make_shared<UiLabel>("JetBrainsMono-VariableFont_wght", "DebugImage2Label");
        mImage2Label->Initialize();
        mImage2Label->SetParents(mCanvas, mImage2Container);
        mImage2Label->SetTextColor(0xFFFFFF);
        mImage2Label->SetFontSize(15);
        mImage2Label->SetTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT);
        mImage2Label->SetTextVerticalAlignment(eTextVerticalAlignmentType::TOP);
        mImage2Label->SetZOrder(2);

        mImage1 = std::make_shared<UiImage>("DebugPanelImage1");
        mImage1->Initialize();
        mImage1->SetParents(mCanvas, mImage1Container);
        mImage1->SetOpacity(1);
        mImage1->SetZOrder(2);

        mImage2 = std::make_shared<UiImage>("DebugPanelImage2");
        mImage2->Initialize();
        mImage2->SetParents(mCanvas, mImage2Container);
        mImage2->SetOpacity(1);
        mImage2->SetZOrder(2);

        mNextPoolsArrowImage = std::make_shared<UiImage>("DebugPanelArrowRightImage");
        mNextPoolsArrowImage->Initialize();
        mNextPoolsArrowImage->SetParents(mCanvas, mTexturesLayout);
        mNextPoolsArrowImage->SetOpacity(1);
        mNextPoolsArrowImage->SetZOrder(2);
        mNextPoolsArrowImage->SetTextureSrc("arrow_right_1.png");

        const auto& arrowMouseInputReceiver = std::make_shared<UiMouseInputReceiverBase>(mNextPoolsArrowImage);
        arrowMouseInputReceiver->SetMouseClickedCallback(
            std::bind(&DebugUiController::OnNextPoolButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
        mNextPoolsArrowImage->SetMouseInputReceiver(arrowMouseInputReceiver);

        mImagePairs.emplace_back(mImage1Label, mImage1);
        mImagePairs.emplace_back(mImage2Label, mImage2);

        RecalculateWidgetsSize();
    }
}

void DebugUiController::Tick(const float deltaTimeSec)
{
}

void DebugUiController::UnpausableTick(const float deltaTimeSec)
{
    const auto& keyboardBindings = mInputComponent->GetKeyboardBindings();
    static constexpr float buttonCooldown = 0.5f;

    if (keyboardBindings->HasPressedKeys()) {
        if (KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::Shift)
            && KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::P)) {
            mCanvas->SetIsVisible(false);
            mPressButtonCooldown = 0.0f;
        } else if (KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::P)) {
            if (mPressButtonCooldown >= buttonCooldown) {
                mPressButtonCooldown = 0.0f;
                if (!mCanvas->IsVisible()) {
                    mCanvas->SetIsVisible(true);
                }
                const auto& activePool = mPools[mPoolIndex];
                for (const auto& [label, image] : mImagePairs) {
                    GoToNextTexture();
                    image->SetTexture(GetCurrentTexture());
                    label->SetText(GetCurrentTextureName());
                }
            }
        }
    }

    mPressButtonCooldown += deltaTimeSec;
}

void DebugUiController::GoToNextTexture()
{
    const auto& activePool = mPools[mPoolIndex];
    const auto totalCount = activePool->GetTexturesCount();
    mTextureIndex = mTextureIndex >= (totalCount - 1) ? 0 : mTextureIndex + 1;
}

std::shared_ptr<ITexture> DebugUiController::GetCurrentTexture() const
{
    const auto& activePool = mPools[mPoolIndex];
    return activePool->GetTextureAt(mTextureIndex);
}

std::string DebugUiController::GetCurrentTextureName() const
{
    const auto& activePool = mPools[mPoolIndex];
    return activePool->GetTextureName(GetCurrentTexture());
}

void DebugUiController::OnNextPoolButtonClicked(const std::weak_ptr<UiItemBase>& senderWp, const glm::ivec2& mouseCursorPosition)
{
    mTextureIndex = -1;
    if ((++mPoolIndex) >= mPools.size()) {
        mPoolIndex = 0;
    }

    for (const auto& [label, image] : mImagePairs) {
        GoToNextTexture();
        image->SetTexture(GetCurrentTexture());
        label->SetText(GetCurrentTextureName());
    }
}

void DebugUiController::SetRenderFpsText(const std::string& fpsText)
{
    if (mCanvas && mCanvas->IsVisible() && mRenderFpsLabel && mRenderFpsLabel->IsVisible()) {
        mRenderFpsLabel->SetText("Render FPS: " + fpsText);
    }
}

void DebugUiController::SetGameFpsText(const std::string& fpsText)
{
    if (mCanvas && mCanvas->IsVisible() && mGameFpsLabel && mGameFpsLabel->IsVisible()) {
        mGameFpsLabel->SetText("Game FPS: " + fpsText);
    }
}

void DebugUiController::SetLuaFpsText(const std::string& fpsText)
{
    if (mCanvas && mCanvas->IsVisible() && mLuaFpsLabel && mLuaFpsLabel->IsVisible()) {
        mLuaFpsLabel->SetText("Lua FPS: " + fpsText);
    }
}

void DebugUiController::ProcessEvent(
    const WindowSizeChangedGameThreadEvent* sender, const WindowSizeChangedGameThreadEvent::EventData_t& data)
{
    RecalculateWidgetsSize();
}
} // namespace Debug
} // namespace EngineCore