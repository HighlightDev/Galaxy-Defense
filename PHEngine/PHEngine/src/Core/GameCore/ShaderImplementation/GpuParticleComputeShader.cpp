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
} // namespace EngineCore::ShaderImpl