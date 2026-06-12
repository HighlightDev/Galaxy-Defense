#include "DepthCollectShader.h"

namespace EngineCore {
namespace ShaderImpl {
DepthCollectShader::DepthCollectShader(const ShaderParams& params)
    : Shader(params)
{
}

void DepthCollectShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{

    Shader::AccessAllUniformLocations(shaderProgramId);

    u_lightWorldPosition = GetUniform("lightWorldPosition", shaderProgramId);
    u_invShadowDistance = GetUniform("invShadowDistance", shaderProgramId);
    u_bWriteDepthLinearly = GetUniform("bWriteDepthLinearly", shaderProgramId);
}

void DepthCollectShader::SetLightWorldPosition(const glm::vec3& position)
{
    u_lightWorldPosition.LoadUniform(position);
}

void DepthCollectShader::SetInvShadowDistance(const float invShadowDistance)
{
    u_invShadowDistance.LoadUniform(invShadowDistance);
}

void DepthCollectShader::SetWriteDepthLinearly(const bool value)
{
    u_bWriteDepthLinearly.LoadUniform(value);
}

void DepthCollectShader::SetShaderPredefine()
{
}
} // namespace ShaderImpl
} // namespace EngineCore