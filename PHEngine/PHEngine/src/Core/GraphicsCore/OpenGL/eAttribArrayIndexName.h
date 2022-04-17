#pragma once

#include <cstdint>
#include <stdint.h>

namespace Graphics
{
    namespace OpenGL
    {
        enum class eAttribArrayIndexName : int8_t
        {
            POSITION_INDICES = -1,
            POSITION = 0,
            NORMAL = 1,
            TEXTURE_COORDINATES = 2,
            COLOR = 3,
            TANGENT = 4,
            BITANGENT = 5,
            BONE_WEIGHT = 6,
            BONE_INDEX = 7,
            CUSTOM_0 = 8,
            CUSTOM_1 = 9,
            CUSTOM_2 = 10
        };
    }
}