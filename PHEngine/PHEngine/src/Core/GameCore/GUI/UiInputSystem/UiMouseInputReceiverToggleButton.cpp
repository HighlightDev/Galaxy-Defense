#include "UiMouseInputReceiverToggleButton.h"

#include "Core/GameCore/GUI/UiElements/UiToggleButton.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
namespace GUI {
UiMouseInputReceiverToggleButton::UiMouseInputReceiverToggleButton(const std::shared_ptr<UiToggleButton>& ownerWp)
    : UiMouseInputReceiverBase(ownerWp)
    , mOwnerWp(ownerWp)
{
}

void UiMouseInputReceiverToggleButton::OnMouseClicked(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (const auto& toggleButtonSp = mOwnerWp.lock()) {
        if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition)) {
            toggleButtonSp->ToggleButton();
            if (mButtonToggledCallback) {
                mButtonToggledCallback(mOwnerWp, toggleButtonSp->IsButtonStateOn());
            }
        }
    }
}

void UiMouseInputReceiverToggleButton::SetButtonToggledCallback(std::function<void(std::weak_ptr<UiToggleButton>, bool)> callback)
{
    mButtonToggledCallback = callback;
}
} // namespace GUI
} // namespace EngineCore
