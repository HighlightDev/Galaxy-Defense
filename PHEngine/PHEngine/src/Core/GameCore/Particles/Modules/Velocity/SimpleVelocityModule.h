#pragma once

#include "Core/GameCore/Particles/Modules/Velocity/IVelocityModule.h"

namespace EngineCore {
class SimpleVelocityModule : public IVelocityModule {

    glm::vec3 mVelocityDirection;
    glm::vec3 mVelocityDeviation;

    glm::vec3 mCurrentSpawnVelocityDeviation;

public:
    SimpleVelocityModule();

    void Update(Particle& particle, const float deltaTime) override;

    void SetVelocityDirection(const glm::vec3& velocityDirection);

    void SetVelocityDeviation(const glm::vec3& velocityDeviation);

    void OnEmitParticles() override;
};
} // namespace EngineCore