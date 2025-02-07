#include "BillboardShader.h"

namespace EngineCore {
namespace ShaderImpl {

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

void BillboardShader::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    Shader::AccessAllUniformLocations(shaderProgramID);

    u_billboardExtent = GetUniform("extent", shaderProgramID);
}
} // namespace ShaderImpl
} // namespace EngineCore