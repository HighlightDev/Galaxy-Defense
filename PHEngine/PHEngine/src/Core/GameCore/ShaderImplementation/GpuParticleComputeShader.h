#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ComputeShaderBase.h"

namespace EngineCore::ShaderImpl {
class GpuParticleComputeShader : public ComputeShaderBase {
private:
public:
    GpuParticleComputeShader(const ShaderParams& params);

    ~GpuParticleComputeShader() override;
};

} // namespace EngineCore::ShaderImpl