#pragma once

#include "Core/GameCore/GUI/UiElements/UiSlider.h"
#include "UiMouseInputReceiverBase.h"

namespace EngineCore {
namespace GUI {
class UiMouseInputReceiverSlider : public UiMouseInputReceiverBase {
protected:
    std::weak_ptr<UiSlider> mOwnerWp;
    std::function<void(std::weak_ptr<UiSlider>, float)> mSliderValueChangedCallback;

public:
    explicit UiMouseInputReceiverSlider(const std::shared_ptr<UiSlider>& ownerWp);

    void SetSliderValueChangedCallback(std::function<void(std::weak_ptr<UiSlider>, float)> callback);

    void OnMousePositionChanged(const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition) override;
};
} // namespace GUI
} // namespace EngineCore