#include "PointLightDepthShader.h"

namespace EngineCore {
namespace ShaderImpl {
PointLightDepthCollectShader::PointLightDepthCollectShader(const ShaderParams& params)
    : Shader(params)
{
}

void PointLightDepthCollectShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Shader::AccessAllUniformLocations(shaderProgramId);
    u_shadowViewMatrices = GetUniformArray("shadowViewMatrices", 6, shaderProgramId, eShaderType::VertexShader);
    u_shadowProjectionMatrices = GetUniformArray("shadowProjectionMatrices", 6, shaderProgramId, eShaderType::VertexShader);
    u_pointLightPos = GetUniform("pointLightPos", shaderProgramId);
    u_farPlane = GetUniform("farPlane", shaderProgramId);
}

void PointLightDepthCollectShader::SetTransformationMatrices(const six_mat4x4& viewMatrices, const six_mat4x4& projectionMatrices)
{
    for (size_t matIndex = 0; matIndex < viewMatrices.size(); ++matIndex) {
        const auto& viewMatrix = viewMatrices[matIndex];
        u_shadowViewMatrices.LoadUniform(matIndex, viewMatrix);
    }

    for (size_t matIndex = 0; matIndex < projectionMatrices.size(); ++matIndex) {
        const auto& projectionMatrix = projectionMatrices[matIndex];
        u_shadowProjectionMatrices.LoadUniform(matIndex, projectionMatrix);
    }
}

void PointLightDepthCollectShader::SetPointLightPosition(const glm::vec3& position)
{
    u_pointLightPos.LoadUniform(position);
}

void PointLightDepthCollectShader::SetFarPlane(const float distance)
{
    u_farPlane.LoadUniform(distance);
}

void PointLightDepthCollectShader::SetShaderPredefine()
{
    DefineConstant<int32_t>(eShaderType::GeometryShader, "CubemapFaces", 6);
}

} // namespace ShaderImpl
} // namespace EngineCore
