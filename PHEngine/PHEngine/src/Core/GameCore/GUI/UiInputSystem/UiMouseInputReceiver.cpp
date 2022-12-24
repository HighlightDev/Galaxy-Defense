#include "UiMouseInputReceiver.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore
{
    namespace GUI
    {
        void UiMouseInputReceiver::SetMouseHoverEnteredCallback(std::function<void(glm::ivec2)> callback)
        {
            mMouseHoverEnteredCallback = callback;
        }

        void UiMouseInputReceiver::SetMouseHoverLeavedCallback(std::function<void(glm::ivec2)> callback)
        {
            mMouseHoverLeavedCallback = callback;
        }

        void UiMouseInputReceiver::SetMouseReleasedCallback(std::function<void(glm::ivec2)> callback)
        {
            mMouseReleasedCallback = callback;
        }

        void UiMouseInputReceiver::SetMousePressedCallback(std::function<void(glm::ivec2)> callback)
        {
            mMousePressedCallback = callback;
        }

        void UiMouseInputReceiver::SetMouseClickedCallback(std::function<void(glm::ivec2)> callback)
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
                        mMouseHoverEnteredCallback(mouseCursorPosition);
                    }
                }
                else
                {
                    if (mWasHoveredLastFrame)
                    {
                        mMouseHoverLeavedCallback(mouseCursorPosition);
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
                    mMouseReleasedCallback(mouseCursorPosition);
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
                    mMousePressedCallback(mouseCursorPosition);
                }
            }
        }

        void UiMouseInputReceiver::OnMouseClicked(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseClickedCallback)
            {
                if (EngineMath::TestPointInAABB(mouseInputArea.GetMin(), mouseInputArea.GetMax(), mouseCursorPosition))
                {
                    mMouseClickedCallback(mouseCursorPosition);
                }
            }
        }
    }
}
