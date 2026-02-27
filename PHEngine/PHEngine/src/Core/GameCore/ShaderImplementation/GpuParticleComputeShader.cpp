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
    m_particleMoveSpeedUniform = GetUniform("particleMoveSpeed", shaderProgramID);
}

void GpuParticleComputeShader::SetDispatchDeltaTime(const double deltaTimeSec)
{
    m_deltaTimeUniform.LoadUniform(static_cast<float>(deltaTimeSec));
}

void GpuParticleComputeShader::SetParticleMoveSpeed(const float moveSpeed)
{
    m_particleMoveSpeedUniform.LoadUniform(moveSpeed);
}

} // namespace EngineCore::ShaderImpl