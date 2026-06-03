#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/GUI/Common/TextEnums.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
struct UiComponentData;

namespace GUI {
class UiCanvas;
class UiLabel;
} // namespace GUI

class UiComponent : public Component {
protected:
    std::shared_ptr<::EngineCore::GUI::UiLabel> mLabel;

    std::shared_ptr<::EngineCore::GUI::UiCanvas> mCanvas;

public:
    UiComponent(const std::shared_ptr<ComponentData>& data);

    ~UiComponent() override;

    eComponentType GetComponentType() const override;

    virtual void CreateUiElements(
        const std::string& fontName,
        const int32_t fontSize,
        const std::string& text,
        const glm::vec3& color,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment,
        const eTextVerticalAlignmentType textVericalAlignment);

    void SetLabelText(const std::string& text);

    void SetLabelVisibility(const bool isVisible);

    void SetLabelTextColor(const glm::vec3& color);

    void SetLabelScreenSpacePosition(const glm::ivec2& position);

    glm::vec2 GetLabelNormalizedSize() const;

    glm::ivec2 GetLabelScreenSpaceSize() const;
};
} // namespace EngineCore