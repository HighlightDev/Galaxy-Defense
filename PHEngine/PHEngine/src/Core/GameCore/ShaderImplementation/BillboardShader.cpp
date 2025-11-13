#include "BillboardShader.h"

namespace EngineCore::ShaderImpl {

BillboardShader::BillboardShader(const ShaderParams& params)
    : Shader(params)
{
}

BillboardShader::~BillboardShader()
{
}

void BillboardShader::SetExtent(const float extent)
{
    u_billboardExtent.LoadUniform(extent);
}

void BillboardShader::SetScreenResolution(const glm::vec2& screenResolution)
{
    u_screenResolution.LoadUniform(screenResolution);
}

void BillboardShader::SetApplyScreenAspectRatio(const bool applyScreenAspectRatio)
{
    u_applyScreenAspectRatio.LoadUniform(applyScreenAspectRatio);
}

void BillboardShader::SetRotationRadians(const float rotationRadians)
{
    u_rotationRadians.LoadUniform(rotationRadians);
}

void BillboardShader::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    Shader::AccessAllUniformLocations(shaderProgramID);

    u_billboardExtent = GetUniform("extent", shaderProgramID);
    u_screenResolution = GetUniform("screenResolution", shaderProgramID);
    u_applyScreenAspectRatio = GetUniform("applyScreenAspectRatio", shaderProgramID);
    u_rotationRadians = GetUniform("rotationRadians", shaderProgramID);
}

} // namespace EngineCore::ShaderImpl