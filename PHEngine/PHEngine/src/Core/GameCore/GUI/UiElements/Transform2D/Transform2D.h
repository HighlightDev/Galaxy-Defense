#pragma once

#include <glm/vec2.hpp>

namespace EngineCore
{
    namespace GUI
    {
        struct Transform2D
        {
            glm::ivec2 Translation;

            Transform2D() : Translation() {}
            Transform2D(const glm::ivec2 &translation) : Translation(translation) {}
        };
    }
}