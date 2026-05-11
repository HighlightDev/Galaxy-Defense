#pragma once

#include "Core/GameCore/Particles/Modules/Velocity/IVelocityModule.h"

namespace EngineCore {

// Moves particles radially outward from the emitter's world-space center.
// In OnEmitParticles the per-particle outward unit vector is stored in
// Particle::InitialVelocity so that CpuParticleSystemComponent uses it as
// the movement direction every tick.
class RadialVelocityModule : public IVelocityModule {
    float mSpeed{1.0f};

public:
    RadialVelocityModule();

    void Update(Particle& particle, const float deltaTimeSec) override;

    void OnEmitParticles() override;

    void EmitSingleParticle(Particle& particle) override;

    void SetSpeed(const float speed);

    float GetSpeed() const;

    std::shared_ptr<IGpuParticleModuleProxy> GetGpuProxy() const override;

private:
    void ProcessEmitSingleParticle(const glm::vec3& particleCenter, Particle& particle);
};
} // namespace EngineCore
