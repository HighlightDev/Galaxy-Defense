#pragma once

#include "Core/GameCore/Particles/Modules/Size/ISizeModule.h"

namespace EngineCore {
class SimpleSizeModule : public ISizeModule {
    float mSizeBegin;

    float mSizeEnd;

public:
    SimpleSizeModule();

    void Update(Particle& particle, const float deltaTimeSec) override;

    void SetSizeBegin(const float sizeBegin);

    void SetSizeEnd(const float sizeEnd);

    void OnEmitParticles() override;

    void EmitSingleParticle(Particle& particle) override;

    std::shared_ptr<IGpuParticleModuleProxy> GetGpuProxy() const override;
};
} // namespace EngineCore