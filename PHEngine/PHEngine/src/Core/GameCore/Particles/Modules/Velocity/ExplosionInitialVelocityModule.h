#pragma once

#include "Core/GameCore/Particles/Modules/Velocity/IVelocityModule.h"

namespace EngineCore {
class ExplosionInitialVelocityModule : public IVelocityModule {
public:
    ExplosionInitialVelocityModule();

    eParticleModuleType GetParticleModuleType() const override;

    void Update(Particle& particle, const float deltaTimeSec) override;

    void OnEmitParticles() override;
};
} // namespace EngineCore