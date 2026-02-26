#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"

namespace EngineCore {
class SimpleSizeModuleGpuProxy : public IGpuParticleModuleProxy {

    float mSizeBegin;
    float mSizeEnd;

public:
    SimpleSizeModuleGpuProxy(const float sizeBegin, const float sizeEnd);

    std::string GetShaderSnippet() const override;

    constexpr uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore
