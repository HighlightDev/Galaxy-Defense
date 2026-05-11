#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ComputeShaderBase.h"

#include <glm/vec3.hpp>

namespace EngineCore::ShaderImpl {
class GpuParticleComputeShader : public ComputeShaderBase {
private:
    Uniform m_deltaTimeUniform, m_isEndlessRespawnEnabledUniform, m_emitterPositionUniform;

public:
    GpuParticleComputeShader(const ShaderParams& params);

    ~GpuParticleComputeShader() override;

    void AccessAllUniformLocations(uint32_t shaderProgramID) override;

    void SetDispatchDeltaTime(const double deltaTimeSec);

    void SetIsEndlessRespawnEnabled(const bool isEnabled);

    void SetEmitterPosition(const glm::vec3& position);
};

} // namespace EngineCore::ShaderImpl