#include "UiMouseInputReceiverBase.h"

#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
namespace GUI {
UiMouseInputReceiverBase::UiMouseInputReceiverBase(const std::shared_ptr<UiItemBase>& ownerWp)
    : mOwnerWp(ownerWp)
{
}

void UiMouseInputReceiverBase::SetMouseHoverEnteredCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
{
    mMouseHoverEnteredCallback = callback;
}

void UiMouseInputReceiverBase::SetMouseHoverLeavedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
{
    mMouseHoverLeavedCallback = callback;
}

void UiMouseInputReceiverBase::SetMouseReleasedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
{
    mMouseReleasedCallback = callback;
}

void UiMouseInputReceiverBase::SetMousePressedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
{
    mMousePressedCallback = callback;
}

void UiMouseInputReceiverBase::SetMouseClickedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
{
    mMouseClickedCallback = callback;
}

void UiMouseInputReceiverBase::OnMousePositionChanged(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition)) {
        if (!mWasHoveredLastFrame) {
            mWasHoveredLastFrame = true;
            if (mMouseHoverEnteredCallback) {
                mMouseHoverEnteredCallback(mOwnerWp, mouseCursorPosition);
            }
        }
    } else if (mWasHoveredLastFrame) {
        mWasHoveredLastFrame = false;
        if (mMouseHoverLeavedCallback) {
            mMouseHoverLeavedCallback(mOwnerWp, mouseCursorPosition);
        }
    }
}

void UiMouseInputReceiverBase::OnMouseReleased(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (mMouseReleasedCallback && mMouseButtonWasPressedLastFrame) {
        mMouseReleasedCallback(mOwnerWp, mouseCursorPosition);
    }
    mMouseButtonWasPressedLastFrame = false;
}

void UiMouseInputReceiverBase::OnMousePressed(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition)) {
        mMouseButtonWasPressedLastFrame = true;
        if (mMousePressedCallback) {
            mMousePressedCallback(mOwnerWp, mouseCursorPosition);
        }
    }
}

void UiMouseInputReceiverBase::OnMouseClicked(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (mMouseClickedCallback) {
        if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition)) {
            mMouseClickedCallback(mOwnerWp, mouseCursorPosition);
        }
    }
}
} // namespace GUI
} // namespace EngineCore
