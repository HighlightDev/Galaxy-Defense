#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"

namespace EngineCore {
class SimpleLifeTimeModuleGpuProxy : public IGpuParticleModuleProxy {

    float mLifeTime;

public:
    explicit SimpleLifeTimeModuleGpuProxy(const float lifeTime);

    std::string GetShaderSnippet() const override;

    uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore
