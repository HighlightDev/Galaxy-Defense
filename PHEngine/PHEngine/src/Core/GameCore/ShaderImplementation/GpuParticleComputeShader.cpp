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
    m_isEndlessRespawnEnabledUniform = GetUniform("isEndlessRespawnEnabled", shaderProgramID);
    m_emitterPositionUniform = GetUniform("emitterPosition", shaderProgramID);
}

void GpuParticleComputeShader::SetDispatchDeltaTime(const double deltaTimeSec)
{
    m_deltaTimeUniform.LoadUniform(static_cast<float>(deltaTimeSec));
}

void GpuParticleComputeShader::SetIsEndlessRespawnEnabled(const bool isEnabled)
{
    m_isEndlessRespawnEnabledUniform.LoadUniform(isEnabled);
}

void GpuParticleComputeShader::SetEmitterPosition(const glm::vec3& position)
{
    m_emitterPositionUniform.LoadUniform(position);
}

} // namespace EngineCore::ShaderImpl