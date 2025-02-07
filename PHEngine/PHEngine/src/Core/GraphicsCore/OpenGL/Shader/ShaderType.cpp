#include "Shader.h"

namespace Graphics {
namespace OpenGL {
int32_t MapShaderTypeToOpenGLConstant(const eShaderType internalShaderType)
{
    switch (internalShaderType) {
    case eShaderType::VertexShader:
        return GL_VERTEX_SHADER;
    case eShaderType::FragmentShader:
        return GL_FRAGMENT_SHADER;
    case eShaderType::GeometryShader:
        return GL_GEOMETRY_SHADER;
    case eShaderType::TesselationControlShader:
        return GL_TESS_CONTROL_SHADER;
    case eShaderType::TesselationEvaluationShader:
        return GL_TESS_EVALUATION_SHADER;

    default: {
        assert(false);
        return 0;
    }
    }
}
} // namespace OpenGL
} // namespace Graphics