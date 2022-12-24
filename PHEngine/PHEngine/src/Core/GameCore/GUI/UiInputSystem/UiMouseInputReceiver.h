#pragma once

#include "IUiMouseInputReceivable.h"

#include <functional>
#include <glm/vec2.hpp>

namespace EngineCore
{
    namespace GUI
    {
        class UiMouseInputReceiver : public IUiMouseInputReceivable
        {
            std::function<void(glm::ivec2)> mMouseHoverEnteredCallback;

            std::function<void(glm::ivec2)> mMouseHoverLeavedCallback;

            std::function<void(glm::ivec2)> mMouseReleasedCallback;

            std::function<void(glm::ivec2)> mMousePressedCallback;

            std::function<void(glm::ivec2)> mMouseClickedCallback;

            bool mWasHoveredLastFrame{false};

            bool mMouseButtonWasPressedLastFrame{false};

        public:

            void SetMouseHoverEnteredCallback(std::function<void(glm::ivec2)> callback);

            void SetMouseHoverLeavedCallback(std::function<void(glm::ivec2)> callback);

            void SetMouseReleasedCallback(std::function<void(glm::ivec2)> callback);

            void SetMousePressedCallback(std::function<void(glm::ivec2)> callback);

            void SetMouseClickedCallback(std::function<void(glm::ivec2)> callback);

            void OnMousePositionChanged(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;

            void OnMouseReleased(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;

            void OnMousePressed(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;

            void OnMouseClicked(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;
        };
    }
}