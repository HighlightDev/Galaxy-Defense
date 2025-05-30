#include "UiMouseInputReceiverSlider.h"

#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiSlider.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
namespace GUI {
UiMouseInputReceiverSlider::UiMouseInputReceiverSlider(const std::shared_ptr<UiSlider>& ownerWp)
    : UiMouseInputReceiverBase(ownerWp)
    , mOwnerWp(ownerWp)
{
}

void UiMouseInputReceiverSlider::SetSliderValueChangedCallback(std::function<void(std::weak_ptr<UiSlider>, float)> callback)
{
    mSliderValueChangedCallback = callback;
}

void UiMouseInputReceiverSlider::OnMousePositionChanged(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (const auto sliderSp = mOwnerWp.lock()) {
        if (mMouseButtonWasPressedLastFrame) {
            if (const auto& canvasSp = sliderSp->GetParentCanvas().lock()) {
                const BoundingBox2D<glm::ivec2> simplifiedInputArea
                    = UiSlider::eUiSliderType::Horizontal == sliderSp->GetSliderType()
                    ? BoundingBox2D<glm::ivec2>(
                          mouseInputArea.GetOrigin(), glm::ivec2(mouseInputArea.GetHalfExtent().x, canvasSp->GetHeight() / 2))
                    : BoundingBox2D<glm::ivec2>(
                          mouseInputArea.GetOrigin(), glm::ivec2((canvasSp->GetWidth() / 2), mouseInputArea.GetHalfExtent().y));

                if (EngineMath::TestPointInAABB(
                        simplifiedInputArea.GetMin(), simplifiedInputArea.GetMax(), mouseCursorPosition)) {
                    const auto newValue = sliderSp->GetValueFromMousePosition(mouseCursorPosition);
                    sliderSp->SetSliderValue(newValue);
                    if (mSliderValueChangedCallback) {
                        mSliderValueChangedCallback(mOwnerWp, newValue);
                    }
                }
            }
        }
    }
}

} // namespace GUI
} // namespace EngineCore