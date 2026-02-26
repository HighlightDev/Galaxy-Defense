#pragma once

#include <string>

namespace EngineCore {
class IGpuParticleModuleProxy {
public:
    virtual ~IGpuParticleModuleProxy() = default;

    virtual std::string GetShaderSnippet() const = 0;

    virtual constexpr uint64_t GetModuleTypeHash() const = 0;
};
} // namespace EngineCore
