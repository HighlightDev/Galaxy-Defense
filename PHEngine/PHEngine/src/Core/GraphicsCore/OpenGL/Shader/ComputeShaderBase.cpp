#include "ComputeShaderBase.h"

#include <gl/glew.h>

using namespace EngineUtility;
using namespace Graphics;

namespace EngineCore {
namespace ShaderImpl {
ComputeShaderBase::ComputeShaderBase(const ShaderParams& params)
    : Shader(params)
{
    ShaderInit();
}

ComputeShaderBase::~ComputeShaderBase()
{
}

void ComputeShaderBase::Dispatch(const uint32_t numGroupsX, const uint32_t numGroupsY, const uint32_t numGroupsZ) const
{
    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

void ComputeShaderBase::SetMemoryBarrier(const eMemoryBarrierType barrierFlags) const
{
    glMemoryBarrier(barrierFlags);
}
} // namespace ShaderImpl
} // namespace EngineCore