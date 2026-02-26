#pragma once

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"
#include "Core/GameCore/Particles/Modules/Velocity/OrbitVelocityModule.h"

namespace EngineCore {

class OrbitVelocityModuleGpuProxy : public IGpuParticleModuleProxy {

    eOrbitExtraVelocityDirectionType mExtraVelocityDirectionType{eOrbitExtraVelocityDirectionType::None};

    float mExtraVelocityPower{0.0f};

public:
    explicit OrbitVelocityModuleGpuProxy(
        const eOrbitExtraVelocityDirectionType extraVelocityDirectionType = eOrbitExtraVelocityDirectionType::None,
        const float extraVelocityPower = 0.0f);

    std::string GetShaderSnippet() const override;

    constexpr uint64_t GetModuleTypeHash() const override;
};
} // namespace EngineCore