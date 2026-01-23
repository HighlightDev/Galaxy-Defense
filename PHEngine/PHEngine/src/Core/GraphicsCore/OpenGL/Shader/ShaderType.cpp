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
    case eShaderType::ComputeShader:
        return GL_COMPUTE_SHADER;

    default: {
        ext_assert(false, "MapShaderTypeToOpenGLConstant: Unsupported shader type: " + ShaderTypeToString(internalShaderType));
        return 0;
    }
    }
}

std::string ShaderTypeToString(const eShaderType shaderType)
{
    switch (shaderType) {
    case eShaderType::VertexShader:
        return "VertexShader";
    case eShaderType::FragmentShader:
        return "FragmentShader";
    case eShaderType::GeometryShader:
        return "GeometryShader";
    case eShaderType::TesselationControlShader:
        return "TesselationControlShader";
    case eShaderType::TesselationEvaluationShader:
        return "TesselationEvaluationShader";
    case eShaderType::ComputeShader:
        return "ComputeShader";
    default:
        return "UnknownShaderType";
    }
}
} // namespace OpenGL
} // namespace Graphics