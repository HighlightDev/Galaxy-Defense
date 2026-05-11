#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"

namespace EngineCore {
class RadialVelocityModuleGpuProxy : public IGpuParticleModuleProxy {
    float mSpeed{1.0f};

public:
    explicit RadialVelocityModuleGpuProxy(const float speed);

    std::string GetShaderSnippet() const override;

    uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore
