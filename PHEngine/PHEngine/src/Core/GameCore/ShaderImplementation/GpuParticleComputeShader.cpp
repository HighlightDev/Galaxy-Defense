#include "GpuParticleComputeShader.h"

using namespace EngineUtility;
using namespace Graphics;

namespace EngineCore::ShaderImpl {
GpuParticleComputeShader::GpuParticleComputeShader(const ShaderParams& params)
    : ComputeShaderBase(params)
{
    ShaderInit();
}

GpuParticleComputeShader::~GpuParticleComputeShader()
{
}

void GpuParticleComputeShader::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    ComputeShaderBase::AccessAllUniformLocations(shaderProgramID);
    m_deltaTimeUniform = GetUniform("deltaTimeSec", shaderProgramID);
    m_isEndlessRespawnEnabledUniform = GetUniform("mIsEndlessRespawnEnabled", shaderProgramID);
}

void GpuParticleComputeShader::SetDispatchDeltaTime(const double deltaTimeSec)
{
    m_deltaTimeUniform.LoadUniform(static_cast<float>(deltaTimeSec));
}

void GpuParticleComputeShader::SetIsEndlessRespawnEnabled(const bool isEnabled)
{
    m_isEndlessRespawnEnabledUniform.LoadUniform(isEnabled);
}

} // namespace EngineCore::ShaderImpl