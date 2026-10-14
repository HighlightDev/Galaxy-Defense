#pragma once

#include <stdint.h>

#include <cstdint>

namespace Graphics {
namespace OpenGL {
enum class eAttribArrayIndex : int8_t {
    PositionIndex = -1,
    VertexPosition = 0,
    VertexNormal = 1,
    VertexTexCoords = 2,
    VertexTangent = 3,
    VertexBitangent = 4,
    VertexBlendWeights = 5,
    VertexBlendIndex = 6,
    VertexColor = 7
};
}
} // namespace Graphics