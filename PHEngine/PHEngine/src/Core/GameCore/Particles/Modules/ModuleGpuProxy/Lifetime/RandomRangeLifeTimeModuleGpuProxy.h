#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"

namespace EngineCore {
class RandomRangeLifeTimeModuleGpuProxy : public IGpuParticleModuleProxy {
    float mMinLifeTime;
    float mMaxLifeTime;

public:
    RandomRangeLifeTimeModuleGpuProxy(const float minLifeTime, const float maxLifeTime);

    std::string GetShaderSnippet() const override;

    uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore
