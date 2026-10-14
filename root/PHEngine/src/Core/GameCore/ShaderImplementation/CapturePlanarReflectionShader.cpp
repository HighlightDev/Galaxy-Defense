#include "CapturePlanarReflectionShader.h"

namespace EngineCore {
namespace ShaderImpl {

CapturePlanarReflectionShader::CapturePlanarReflectionShader(const ShaderParams& params)
    : Shader(params)
{
}

CapturePlanarReflectionShader::~CapturePlanarReflectionShader()
{
}

void CapturePlanarReflectionShader::SetClipPlane(const glm::vec4& plane)
{
    uClipPlane.LoadUniform(plane);
}

void CapturePlanarReflectionShader::SetShaderPredefine()
{
}

void CapturePlanarReflectionShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{

    Base::AccessAllUniformLocations(shaderProgramId);
    uClipPlane = GetUniform("clipPlane", shaderProgramId);
}

} // namespace ShaderImpl
} // namespace EngineCore