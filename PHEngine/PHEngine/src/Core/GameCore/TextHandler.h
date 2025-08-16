#pragma once

#include "Core/GameCore/GUI/HudText/HudTextField.h"

#include <memory>
#include <vector>

namespace EngineCore {
class Scene;

class TextHandler {
    std::weak_ptr<Scene> mSceneWp;

    std::vector<std::shared_ptr<HudTextField>> mRegisteredTexts;

public:
    TextHandler();

    ~TextHandler();

    void CleanUp();

    void Initialize();

    void SetScene(const std::weak_ptr<Scene>& sceneWp);

    std::shared_ptr<HudTextField> GetTextFieldById(const int32_t fieldId) const;

    std::shared_ptr<HudTextField> CreateTextField(
        const std::string& fontName,
        const int32_t fontSize,
        const std::string& text,
        const glm::vec3& color,
        const glm::vec2& position,
        const bool receiveUpdateOnTextScreenSpaceSizeChanged,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment);

    std::shared_ptr<HudTextField> CreateEmptyTextField(
        const std::string& fontName,
        const int32_t fontSize,
        const glm::vec3& color,
        const bool receiveUpdateOnTextScreenSpaceSizeChanged,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment);

    void UnregisterText(const int32_t fieldId);

    void SetText(const int32_t textFieldId, const std::string& text);

    void SetVisibility(const int32_t textFieldId, const bool isVisible);

    void SetColor(const int32_t textFieldId, const glm::vec3& color);

    void SetPosition(const int32_t textFieldId, const glm::vec2& position);

private:
    void RegisterTextField(const std::shared_ptr<HudTextField>& textField, const bool receiveUpdateOnTextScreenSpaceSizeChanged);

    void UnregisterTextField(const std::shared_ptr<HudTextField>& textField);

    void OnTextDataChanged(const std::shared_ptr<HudTextField>& textField, const eTextChangedDataType changedDataType);
};

} // namespace EngineCore