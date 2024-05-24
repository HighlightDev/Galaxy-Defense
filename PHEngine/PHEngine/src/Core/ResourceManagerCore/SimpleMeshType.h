#pragma once

#include <stdint.h>
#include <cstdint>

namespace Resources
{
    enum class SimplePrimitiveType : int8_t
    {
        POINT,
        CUBE,
        QUAD,
        INVERTED_VERTICES_DIRECTION_CUBE,
        PLANE,
        PLANE_WITH_ATTRIBUTES
    };
}
