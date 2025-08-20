#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
struct ComponentData;

class UiComponent : public Component {
public:
    UiComponent(const std::shared_ptr<ComponentData>& data);

    ~UiComponent() override;

    eComponentType GetComponentType() const override;

    void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

    int32_t CreateTextField(
        const std::string& fontName,
        const int32_t fontSize,
        const std::string& text,
        const glm::vec3& color,
        const glm::vec2& position,
        const bool receiveUpdateOnTextScreenSpaceSizeChanged,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment,
        const eTextVerticalAlignmentType textVericalAlignment);

    int32_t CreateEmptyTextField(
        const std::string& fontName,
        const int32_t fontSize,
        const glm::vec3& color,
        const bool receiveUpdateOnTextScreenSpaceSizeChanged,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment,
        const eTextVerticalAlignmentType textVericalAlignment);

    void DeleteTextField(const int32_t textFieldId);

    std::shared_ptr<HudTextField> GetTextFieldById(const int32_t textFieldId) const;

    void SetText(const int32_t textFieldId, const std::string& text);

    void SetVisibility(const int32_t textFieldId, const bool isVisible);

    void SetColor(const int32_t textFieldId, const glm::vec3& color);

    void SetPosition(const int32_t textFieldId, const glm::vec2& position);
};
} // namespace EngineCore