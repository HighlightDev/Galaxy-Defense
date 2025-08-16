#include "UiComponent.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/TextHandler.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

#include <algorithm>

using namespace Graphics::Renderer;

namespace EngineCore {
UiComponent::UiComponent(const std::shared_ptr<ComponentData>& data)
    : Component(data->EngineObjectName)
{
}

UiComponent::~UiComponent()
{
}

eComponentType UiComponent::GetComponentType() const
{
    return eComponentType::UI_COMPONENT;
}

void UiComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
{
}

int32_t UiComponent::CreateTextField(
    const std::string& fontName,
    const int32_t fontSize,
    const std::string& text,
    const glm::vec3& color,
    const glm::vec2& position,
    const bool receiveUpdateOnTextScreenSpaceSizeChanged,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        const auto& textFieldSp = textHanderSp->CreateTextField(
            fontName,
            fontSize,
            text,
            color,
            position,
            receiveUpdateOnTextScreenSpaceSizeChanged,
            lineMaxWidthHeight,
            textHorizontalAlignment);
        return textFieldSp->GetTextFieldId();
    }

    return -1;
}

int32_t UiComponent::CreateEmptyTextField(
    const std::string& fontName,
    const int32_t fontSize,
    const glm::vec3& color,
    const bool receiveUpdateOnTextScreenSpaceSizeChanged,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        const auto& textFieldSp = textHanderSp->CreateEmptyTextField(
            fontName,
            fontSize,
            color,
            receiveUpdateOnTextScreenSpaceSizeChanged,
            lineMaxWidthHeight,
            textHorizontalAlignment);
        return textFieldSp->GetTextFieldId();
    }

    return -1;
}

void UiComponent::DeleteTextField(const int32_t textFieldId)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        textHanderSp->UnregisterText(textFieldId);
    }
}

std::shared_ptr<HudTextField> UiComponent::GetTextFieldById(const int32_t textFieldId) const
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        return textHanderSp->GetTextFieldById(textFieldId);
    }

    return nullptr;
}

void UiComponent::SetText(const int32_t textFieldId, const std::string& text)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        textHanderSp->SetText(textFieldId, text);
    }
}

void UiComponent::SetVisibility(const int32_t textFieldId, const bool isVisible)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        textHanderSp->SetVisibility(textFieldId, isVisible);
    }
}

void UiComponent::SetColor(const int32_t textFieldId, const glm::vec3& color)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        textHanderSp->SetColor(textFieldId, color);
    }
}

void UiComponent::SetPosition(const int32_t textFieldId, const glm::vec2& position)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        const auto& textHanderSp = sceneSp->GetTextHandler();
        textHanderSp->SetPosition(textFieldId, position);
    }
}
} // namespace EngineCore