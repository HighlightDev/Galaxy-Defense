#include "UiComponent.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/UiComponentData.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

#include <algorithm>

using namespace Graphics::Renderer;

namespace EngineCore {
UiComponent::UiComponent(const std::shared_ptr<ComponentData>& data)
    : Component(data->EngineObjectName)
{
    const auto uiData = std::dynamic_pointer_cast<UiComponentData>(data);
    ext_assert(uiData != nullptr, "UiComponent::UiComponent: ComponentData is not of type UiComponentData");
    mCanvas = uiData->Canvas;
}

UiComponent::~UiComponent()
{
}

eComponentType UiComponent::GetComponentType() const
{
    return eComponentType::UI_COMPONENT;
}

void UiComponent::CreateUiElements(
    const std::string& fontName,
    const int32_t fontSize,
    const std::string& text,
    const glm::vec3& color,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const eTextVerticalAlignmentType textVericalAlignment)
{
    if (mLabel == nullptr) {
        mLabel = std::make_shared<UiLabel>(fontName, "UiComponentLabel");
        mLabel->Initialize();
        mLabel->SetParents(mCanvas, mCanvas);
        mLabel->SetTextColor(color);
        mLabel->SetFontSize(fontSize);
        mLabel->SetWidth(lineMaxWidthHeight.x);
        mLabel->SetHeight(lineMaxWidthHeight.y);
        mLabel->SetZOrder(50);
        mLabel->SetText(text);
        mLabel->SetTextColor(color);
        mLabel->SetTextHorizontalAlignment(textHorizontalAlignment);
        mLabel->SetTextVerticalAlignment(textVericalAlignment);
    } else {
        LogInfo("Warning: attempt to create SpaceObjectHealthBar while it is already created.");
    }
}

void UiComponent::SetLabelText(const std::string& text)
{
    if (mLabel) {
        mLabel->SetText(text);
    }
}

void UiComponent::SetLabelVisibility(const bool isVisible)
{
    if (mLabel) {
        mLabel->SetIsVisible(isVisible);
    }
}

void UiComponent::SetLabelTextColor(const glm::vec3& color)
{
    if (mLabel) {
        mLabel->SetTextColor(color);
    }
}

void UiComponent::SetLabelScreenSpacePosition(const glm::ivec2& position)
{
    if (mLabel) {
        mLabel->SetAbsoluteOrigin(position);
    }
}

glm::vec2 UiComponent::GetLabelNormalizedSize() const
{
    if (mLabel) {
        return mLabel->GetTextNormalizedSize();
    }
    return glm::vec2(0.0f);
}

glm::ivec2 UiComponent::GetLabelScreenSpaceSize() const
{
    if (mLabel) {
        return mLabel->GetTextScreenSpaceSize();
    }
    return glm::ivec2(0);
}
} // namespace EngineCore