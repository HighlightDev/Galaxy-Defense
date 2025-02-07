#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"

#include <glm/vec2.hpp>

namespace EngineCore {
namespace GUI {
class IUiMouseInputReceivable {
public:
    virtual void OnMousePositionChanged(const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition)
        = 0;

    virtual void OnMouseReleased(const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition) = 0;

    virtual void OnMousePressed(const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition) = 0;

    virtual void OnMouseClicked(const BoundingBox2D<glm::ivec2>& mouseInputArea, const glm::ivec2& mouseCursorPosition) = 0;
};
} // namespace GUI
} // namespace EngineCore