#include "UiMouseInputReceiver.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

namespace EngineCore
{
    namespace GUI
    {
        UiMouseInputReceiver::UiMouseInputReceiver(const std::weak_ptr<UiItemBase>& ownerWp)
         : mOwnerWp(ownerWp)
        {
        }

        void UiMouseInputReceiver::SetMouseHoverEnteredCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
        {
            mMouseHoverEnteredCallback = callback;
        }

        void UiMouseInputReceiver::SetMouseHoverLeavedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
        {
            mMouseHoverLeavedCallback = callback;
        }

        void UiMouseInputReceiver::SetMouseReleasedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
        {
            mMouseReleasedCallback = callback;
        }

        void UiMouseInputReceiver::SetMousePressedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
        {
            mMousePressedCallback = callback;
        }

        void UiMouseInputReceiver::SetMouseClickedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback)
        {
            mMouseClickedCallback = callback;
        }

        void UiMouseInputReceiver::OnMousePositionChanged(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseHoverEnteredCallback && mMouseHoverLeavedCallback)
            {
                if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition))
                {
                    if (!mWasHoveredLastFrame)
                    {
                        mWasHoveredLastFrame = true;
                        mMouseHoverEnteredCallback(mOwnerWp, mouseCursorPosition);
                    }
                }
                else
                {
                    if (mWasHoveredLastFrame)
                    {
                        mMouseHoverLeavedCallback(mOwnerWp, mouseCursorPosition);
                        mWasHoveredLastFrame = false;
                    }
                }
            }
        }

        void UiMouseInputReceiver::OnMouseReleased(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseReleasedCallback)
            {
                if (mMouseButtonWasPressedLastFrame)
                {
                    mMouseReleasedCallback(mOwnerWp, mouseCursorPosition);
                    mMouseButtonWasPressedLastFrame = false;
                }
            }
        }

        void UiMouseInputReceiver::OnMousePressed(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition)
        {
            if (mMousePressedCallback)
            {
                if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition))
                {
                    mMouseButtonWasPressedLastFrame = true;
                    mMousePressedCallback(mOwnerWp, mouseCursorPosition);
                }
            }
        }

        void UiMouseInputReceiver::OnMouseClicked(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseClickedCallback)
            {
                if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition))
                {
                    mMouseClickedCallback(mOwnerWp, mouseCursorPosition);
                }
            }
        }
    }
}
