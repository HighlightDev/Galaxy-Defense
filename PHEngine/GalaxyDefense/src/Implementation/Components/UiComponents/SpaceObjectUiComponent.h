#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"

using namespace EngineCore;

namespace EngineCore {
namespace GUI {
class UiCanvas;
class UiProgressBar;
} // namespace GUI
} // namespace EngineCore

namespace EngineCore {
class SceneComponent;
}

namespace Game {
class SpaceObjectUiComponent : public UiComponent {

    std::weak_ptr<::EngineCore::SceneComponent> mOwnerRootComponent;

    std::shared_ptr<::EngineCore::GUI::UiProgressBar> mHealthBar;

public:
    explicit SpaceObjectUiComponent(const std::shared_ptr<ComponentData>& data);

    ~SpaceObjectUiComponent() override;

    void Tick(const float deltaTime) override;

    void CreateUiElements(
        const std::string& fontName,
        const int32_t fontSize,
        const std::string& text,
        const glm::vec3& color,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment,
        const eTextVerticalAlignmentType textVericalAlignment) override;

    void SetHealthBarVisibility(const bool isVisible);

    void FadeIn();

    void FadeOut();

    void SetHealthBarFillPercent(const float fillPercent);

private:
    glm::ivec2 CalculateHealthBarPosition() const;

    void OnAnimationFinished(const std::string& animationName);
};
} // namespace Game
