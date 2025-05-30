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
    if (mMouseHoverEnteredCallback && mMouseHoverLeavedCallback) {
        if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition)) {
            if (!mWasHoveredLastFrame) {
                mWasHoveredLastFrame = true;
                mMouseHoverEnteredCallback(mOwnerWp, mouseCursorPosition);
            }
        } else {
            if (mWasHoveredLastFrame) {
                mMouseHoverLeavedCallback(mOwnerWp, mouseCursorPosition);
                mWasHoveredLastFrame = false;
            }
        }
    }
}

void UiMouseInputReceiverBase::OnMouseReleased(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (mMouseReleasedCallback) {
        if (mMouseButtonWasPressedLastFrame) {
            mMouseReleasedCallback(mOwnerWp, mouseCursorPosition);
        }
    }
    mMouseButtonWasPressedLastFrame = false;
}

void UiMouseInputReceiverBase::OnMousePressed(
    const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
{
    if (mMousePressedCallback) {
        if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition)) {
            mMousePressedCallback(mOwnerWp, mouseCursorPosition);
        }
    }
    mMouseButtonWasPressedLastFrame = true;
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
