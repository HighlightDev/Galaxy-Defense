#include "BarrierUiComponent.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/GUI/UiElements/UiProgressBar.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"

using namespace EngineCore::GUI;
using namespace EngineCore::DataProviders;

namespace Game {
BarrierUiComponent::BarrierUiComponent(const std::shared_ptr<ComponentData>& data)
    : UiComponent(data)
    , mHealthBar(nullptr)
{
}

BarrierUiComponent::~BarrierUiComponent()
{
    if (mHealthBar) {
        if (const auto& canvasSp = mCanvas) {
            canvasSp->RemoveUiItem(mHealthBar);
        }
    }
}

void BarrierUiComponent::CreateUiElements(
    const std::string& fontName,
    const int32_t fontSize,
    const std::string& text,
    const glm::vec3& color,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const eTextVerticalAlignmentType textVericalAlignment)
{
    UiComponent::CreateUiElements(
        fontName, fontSize, text, color, lineMaxWidthHeight, textHorizontalAlignment, textVericalAlignment);

    if (mHealthBar == nullptr) {
        mHealthBar = std::make_shared<UiProgressBar>("BarrierHealthBar_" + std::to_string(GetObjectId()));
        mHealthBar->Initialize();
        mHealthBar->SetZOrder(100);
        mHealthBar->SetOpacity(0.0f);
        mHealthBar->SetWidth(30);
        mHealthBar->SetHeight(5);
        mHealthBar->SetIsVisible(false);
        mHealthBar->SetEmptyColor(0x000000);
        mHealthBar->SetFilledColor(0x00FF00);
        mHealthBar->SetFillPercentValue(1.0f);
        mHealthBar->SetParents(mCanvas, mCanvas);
        mHealthBar->AddAnimation(
            "FadeIn", AnimationData(eAnimationInterpolationFunctionType::LINEAR, 0.6f, "Opacity", 0.0f, 1.0f));
        mHealthBar->AddAnimation(
            "FadeOut", AnimationData(eAnimationInterpolationFunctionType::LINEAR, 0.6f, "Opacity", 1.0f, 0.0f));
        mHealthBar->GetAnimator()->SubscribeOnAnimationFinished(
            std::bind(&BarrierUiComponent::OnAnimationFinished, this, std::placeholders::_1));
        mHealthBar->GetAnimator()->SetFinishAnimationOnNewAnimationStart(false);

        ext_assert(mLabel != nullptr, "UI label is null in BarrierUiComponent");
        mLabel->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::TOP, mHealthBar->GetName());
        mLabel->SetAnchorMargin(eUiAnchor::BOTTOM, 20);
        mLabel->SetAnchor(eUiAnchor::HORIZONTAL_CENTER, eUiAnchor::HORIZONTAL_CENTER, mHealthBar->GetName());
        mLabel->AddAnimation("FadeIn", AnimationData(eAnimationInterpolationFunctionType::LINEAR, 0.6f, "Opacity", 0.0f, 1.0f));
        mLabel->AddAnimation("FadeOut", AnimationData(eAnimationInterpolationFunctionType::LINEAR, 0.6f, "Opacity", 1.0f, 0.0f));
        mLabel->GetAnimator()->SubscribeOnAnimationFinished(
            std::bind(&BarrierUiComponent::OnAnimationFinished, this, std::placeholders::_1));
        mLabel->GetAnimator()->SetFinishAnimationOnNewAnimationStart(false);
    } else {
        LogInfo("Warning: attempt to create BarrierHealthBar while it is already created.");
    }
}

void BarrierUiComponent::Tick(const float deltaTimeSec)
{
    UiComponent::Tick(deltaTimeSec);

    if (mHealthBar && mHealthBar->IsVisible()) {
        mHealthBar->SetAbsoluteOrigin(CalculateHealthBarPosition());
    }
}

void BarrierUiComponent::SetWorldPosition(const glm::vec3& worldPosition)
{
    mWorldPosition = worldPosition;
}

glm::ivec2 BarrierUiComponent::CalculateHealthBarPosition() const
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& mainCameraSp = sceneSp->GetMainCamera();
        const glm::vec4 clippedSpaceTranslation
            = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(mWorldPosition, 1.0f));
        const glm::vec3 ndcTranslation = glm::vec3(
            clippedSpaceTranslation.x / clippedSpaceTranslation.w,
            clippedSpaceTranslation.y / clippedSpaceTranslation.w,
            clippedSpaceTranslation.z / clippedSpaceTranslation.w);

        const glm::vec2 textureSpaceTranslation
            = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));

        const glm::ivec2 screenSpacePosition = glm::ivec2(
            static_cast<int32_t>(GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth() * textureSpaceTranslation.x),
            static_cast<int32_t>(
                GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight() * (1.0 - textureSpaceTranslation.y)));
        return glm::ivec2(screenSpacePosition.x - mHealthBar->GetWidth() / 2, screenSpacePosition.y + 50);
    }

    return glm::ivec2(0, 0);
}

void BarrierUiComponent::SetHealthBarVisibility(const bool isVisible)
{
    if (mHealthBar != nullptr) {
        mHealthBar->SetIsVisible(isVisible);
    }
}

void BarrierUiComponent::FadeIn()
{
    if (mHealthBar && mHealthBar->GetAnimator()->GetActiveAnimationName() != "FadeIn") {
        SetHealthBarVisibility(true);
        mHealthBar->GetAnimator()->StartAnimation("FadeIn");
    }
    if (mLabel && mLabel->GetAnimator()->GetActiveAnimationName() != "FadeIn") {
        SetLabelVisibility(true);
        mLabel->GetAnimator()->StartAnimation("FadeIn");
    }
}

void BarrierUiComponent::FadeOut()
{
    if (mHealthBar && mHealthBar->GetAnimator()->GetActiveAnimationName() != "FadeOut") {
        mHealthBar->GetAnimator()->StartAnimation("FadeOut");
    }
    if (mLabel && mLabel->GetAnimator()->GetActiveAnimationName() != "FadeOut") {
        mLabel->GetAnimator()->StartAnimation("FadeOut");
    }
}

void BarrierUiComponent::OnAnimationFinished(const std::string& animationName)
{
    if (mHealthBar != nullptr) {
        if ("FadeOut" == animationName) {
            SetHealthBarVisibility(false);
        }
    }
    if (mLabel != nullptr) {
        if ("FadeOut" == animationName) {
            SetLabelVisibility(false);
        }
    }
}

void BarrierUiComponent::SetHealthBarFillPercent(const float fillPercent)
{
    if (mHealthBar != nullptr) {
        mHealthBar->SetFillPercentValue(fillPercent);
    }
}

void BarrierUiComponent::SetHealthBarFilledColor(const uint32_t hexColor)
{
    if (mHealthBar != nullptr) {
        mHealthBar->SetFilledColor(hexColor);
    }
}
} // namespace Game
