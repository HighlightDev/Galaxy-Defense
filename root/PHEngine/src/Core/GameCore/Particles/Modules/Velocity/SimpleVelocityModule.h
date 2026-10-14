#pragma once

#include "Core/GameCore/Particles/Modules/Velocity/IVelocityModule.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class SimpleVelocityModule : public IVelocityModule {

    glm::vec3 mVelocityDirection;
    glm::vec3 mVelocityDeviation;

    glm::vec3 mCurrentSpawnVelocityDeviation;

    float mSpeed{1.0f};

public:
    SimpleVelocityModule();

    void Update(Particle& particle, const float deltaTimeSec) override;

    void SetVelocityDirection(const glm::vec3& velocityDirection);

    void SetVelocityDeviation(const glm::vec3& velocityDeviation);

    void OnEmitParticles() override;

    void EmitSingleParticle(Particle& particle) override;

    void SetSpeed(const float speed);

    std::shared_ptr<IGpuParticleModuleProxy> GetGpuProxy() const override;
};
} // namespace EngineCore