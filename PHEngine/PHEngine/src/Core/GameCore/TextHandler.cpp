#include "TextHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

#include <glm/vec3.hpp>

#include <algorithm>

using namespace Graphics::Renderer;

namespace EngineCore {
TextHandler::TextHandler()
    : mSceneWp()
    , mRegisteredTexts()
{
}

TextHandler::~TextHandler()
{
}

void TextHandler::Initialize()
{
}

void TextHandler::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

std::shared_ptr<HudTextField> TextHandler::CreateTextField(
    const std::string& fontName,
    const int32_t fontSize,
    const std::string& text,
    const glm::vec3& color,
    const glm::vec2& position,
    const bool receiveUpdateOnTextScreenSpaceSizeChanged,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const eTextVerticalAlignmentType textVericalAlignment)
{
    const auto& textField = mRegisteredTexts.emplace_back(std::make_shared<HudTextField>(
        fontName, fontSize, text, color, position, lineMaxWidthHeight, textHorizontalAlignment, textVericalAlignment));

    RegisterTextField(textField, receiveUpdateOnTextScreenSpaceSizeChanged);
    return textField;
}

std::shared_ptr<HudTextField> TextHandler::CreateEmptyTextField(
    const std::string& fontName,
    const int32_t fontSize,
    const glm::vec3& color,
    const bool receiveUpdateOnTextScreenSpaceSizeChanged,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const eTextVerticalAlignmentType textVericalAlignment)
{
    const auto& textField = mRegisteredTexts.emplace_back(std::make_shared<HudTextField>(
        fontName, fontSize, color, lineMaxWidthHeight, textHorizontalAlignment, textVericalAlignment));

    RegisterTextField(textField, receiveUpdateOnTextScreenSpaceSizeChanged);
    return textField;
}

void TextHandler::UnregisterText(const int32_t fieldId)
{
    auto foundIt = std::find_if(mRegisteredTexts.begin(), mRegisteredTexts.end(), [fieldId](const auto& textField) {
        return fieldId == textField->GetTextFieldId();
    });

    const auto& foundTextSp = *foundIt;
    if (foundIt != mRegisteredTexts.end()) {
        UnregisterTextField(*foundIt);
        mRegisteredTexts.erase(std::remove_if(mRegisteredTexts.begin(), mRegisteredTexts.end(), [fieldId](const auto& textField) {
            return fieldId == textField->GetTextFieldId();
        }));
    }
}

void TextHandler::CleanUp()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            for (const auto& textFieldSp : mRegisteredTexts) {
                sceneRendererSp->UnregisterText(textFieldSp->GetTextFieldId());
            }
        }
    }
    mRegisteredTexts.clear();
}

std::shared_ptr<HudTextField> TextHandler::GetTextFieldById(const int32_t fieldId) const
{
    auto foundIt = std::find_if(mRegisteredTexts.begin(), mRegisteredTexts.end(), [=](const auto& textField) {
        return fieldId == textField->GetTextFieldId();
    });

    return foundIt != mRegisteredTexts.end() ? *foundIt : nullptr;
}

void TextHandler::RegisterTextField(
    const std::shared_ptr<HudTextField>& textField, const bool receiveUpdateOnTextScreenSpaceSizeChanged)
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneRendererSp->RegisterText_OnRenderThread(textField, receiveUpdateOnTextScreenSpaceSizeChanged);
        }
    }
}

void TextHandler::UnregisterTextField(const std::shared_ptr<HudTextField>& textField)
{
    assert(textField);
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneRendererSp->UnregisterText_OnRenderThread(textField);
        }
    }
}

void TextHandler::SetText(const int32_t textFieldId, const std::string& text)
{
    const auto& textFieldSp = GetTextFieldById(textFieldId);
    assert(textFieldSp);
    textFieldSp->SetText(text);
    OnTextDataChanged(textFieldSp, eTextChangedDataType::TEXT);
}

void TextHandler::SetVisibility(const int32_t textFieldId, const bool isVisible)
{
    const auto& textFieldSp = GetTextFieldById(textFieldId);
    assert(textFieldSp);
    textFieldSp->SetVisibility(isVisible);
    OnTextDataChanged(textFieldSp, eTextChangedDataType::VISIBILITY);
}

void TextHandler::SetColor(const int32_t textFieldId, const glm::vec3& color)
{
    const auto& textFieldSp = GetTextFieldById(textFieldId);
    assert(textFieldSp);
    textFieldSp->SetColor(color);
    OnTextDataChanged(textFieldSp, eTextChangedDataType::COLOR);
}

void TextHandler::SetPosition(const int32_t textFieldId, const glm::vec2& position)
{
    const auto& textFieldSp = GetTextFieldById(textFieldId);
    assert(textFieldSp);
    textFieldSp->SetPosition(position);
    OnTextDataChanged(textFieldSp, eTextChangedDataType::OFFSET);
}

void TextHandler::OnTextDataChanged(const std::shared_ptr<HudTextField>& textField, const eTextChangedDataType changedDataType)
{
    if (auto sceneSp = mSceneWp.lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneRendererSp->TextDataChanged_OnRenderThread(textField, changedDataType);
        }
    }
}
} // namespace EngineCore