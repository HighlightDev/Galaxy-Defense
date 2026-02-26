#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"

namespace EngineCore {
class ExplosionInitialVelocityModuleGpuProxy : public IGpuParticleModuleProxy {
public:
    ExplosionInitialVelocityModuleGpuProxy();

    std::string GetShaderSnippet() const override;

    constexpr uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore