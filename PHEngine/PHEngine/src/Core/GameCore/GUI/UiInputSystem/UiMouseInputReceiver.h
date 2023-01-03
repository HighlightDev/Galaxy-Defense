#pragma once

#include "IUiMouseInputReceivable.h"

#include <functional>
#include <glm/vec2.hpp>
#include <memory>

namespace EngineCore
{
    namespace GUI
    {
        class UiItemBase;

        class UiMouseInputReceiver : public IUiMouseInputReceivable
        {
            std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> mMouseHoverEnteredCallback;

            std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> mMouseHoverLeavedCallback;

            std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> mMouseReleasedCallback;

            std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> mMousePressedCallback;

            std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> mMouseClickedCallback;

            bool mWasHoveredLastFrame{false};

            bool mMouseButtonWasPressedLastFrame{false};

            std::weak_ptr<UiItemBase> mOwnerWp;

        public:

            explicit UiMouseInputReceiver(const std::weak_ptr<UiItemBase>& ownerWp);

            void SetMouseHoverEnteredCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback);

            void SetMouseHoverLeavedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback);

            void SetMouseReleasedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback);

            void SetMousePressedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback);

            void SetMouseClickedCallback(std::function<void(std::weak_ptr<UiItemBase>, glm::ivec2)> callback);

            void OnMousePositionChanged(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;

            void OnMouseReleased(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;

            void OnMousePressed(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;

            void OnMouseClicked(const BoundingBox2D &mouseInputArea, const glm::ivec2 &mouseCursorPosition) override;
        };
    }
}