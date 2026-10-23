#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"

using namespace EngineCore;

namespace EngineCore::GUI {
class UiCanvas;
class UiProgressBar;
} // namespace EngineCore::GUI

namespace Game {
class BarrierUiComponent : public UiComponent {

    std::shared_ptr<::EngineCore::GUI::UiProgressBar> mHealthBar;

    glm::vec3 mWorldPosition{0.0f};

public:
    explicit BarrierUiComponent(const std::shared_ptr<ComponentData>& data);

    ~BarrierUiComponent() override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void CreateUiElements(
        const std::string& fontName,
        const int32_t fontSize,
        const std::string& text,
        const glm::vec3& color,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment,
        const eTextVerticalAlignmentType textVericalAlignment) override;

    void SetWorldPosition(const glm::vec3& worldPosition);

    void SetHealthBarVisibility(const bool isVisible);

    void FadeIn();

    void FadeOut();

    void SetHealthBarFillPercent(const float fillPercent);

    void SetHealthBarFilledColor(const uint32_t hexColor);

private:
    glm::ivec2 CalculateHealthBarPosition() const;

    void OnAnimationFinished(const std::string& animationName);
};
} // namespace Game
