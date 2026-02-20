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
    m_deltaTimeUniform = GetUniform("deltaTimeSec", shaderProgramID);
    m_particleLifetimeUniform = GetUniform("particleLifetime", shaderProgramID);
}

void GpuParticleComputeShader::SetDispatchDeltaTime(const double deltaTimeSec)
{
    m_deltaTimeUniform.LoadUniform(static_cast<float>(deltaTimeSec));
}

void GpuParticleComputeShader::SetParticleLifetime(const float lifetime)
{
    m_particleLifetimeUniform.LoadUniform(lifetime);
}

} // namespace EngineCore::ShaderImpl