#pragma once
#include "Core/CommonCore/Assertion.h"

#include <gl/glew.h>
#include <stdint.h>

#include <cstddef>

namespace Graphics {
namespace OpenGL {
enum eShaderType : int32_t {
    VertexShader = 0x01,
    FragmentShader = VertexShader << 1,
    GeometryShader = FragmentShader << 1,
    TesselationControlShader = GeometryShader << 1,
    TesselationEvaluationShader = TesselationControlShader << 1,
};

int32_t MapShaderTypeToOpenGLConstant(const eShaderType internalShaderType);
} // namespace OpenGL
} // namespace Graphics