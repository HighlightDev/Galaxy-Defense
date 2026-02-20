#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ComputeShaderBase.h"

namespace EngineCore::ShaderImpl {
class GpuParticleComputeShader : public ComputeShaderBase {
private:
    Uniform m_deltaTimeUniform, m_particleLifetimeUniform;

public:
    GpuParticleComputeShader(const ShaderParams& params);

    ~GpuParticleComputeShader() override;

    void AccessAllUniformLocations(uint32_t shaderProgramID) override;

    void SetDispatchDeltaTime(const double deltaTimeSec);

    void SetParticleLifetime(const float lifetime);
};

} // namespace EngineCore::ShaderImpl