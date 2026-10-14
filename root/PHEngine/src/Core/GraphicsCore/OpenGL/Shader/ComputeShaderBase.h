#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {
class ComputeShaderBase : public Shader {
private:
public:
    ComputeShaderBase(const ShaderParams& params);

    ~ComputeShaderBase() override;

    virtual void Dispatch(const uint32_t numGroupsX, const uint32_t numGroupsY, const uint32_t numGroupsZ) const;

    virtual void SetMemoryBarrier(const eMemoryBarrierType barrierFlags) const;
};

} // namespace ShaderImpl
} // namespace EngineCore
