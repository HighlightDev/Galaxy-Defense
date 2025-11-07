#include "SimpleVelocityModule.h"

#include "Core/CommonCore/Random.h"

namespace EngineCore {
SimpleVelocityModule::SimpleVelocityModule()
    : mVelocityDirection()
    , mVelocityDeviation()
    , mCurrentSpawnVelocityDeviation()
{
}

void SimpleVelocityModule::Update(Particle& particle, const float deltaTimeSec)
{
    particle.Velocity = (mVelocityDirection * deltaTimeSec * mExtraVelocityPower) + (mCurrentSpawnVelocityDeviation * deltaTimeSec);
}

void SimpleVelocityModule::SetVelocityDirection(const glm::vec3& velocityDirection)
{
    mVelocityDirection = velocityDirection;
}

void SimpleVelocityModule::SetVelocityDeviation(const glm::vec3& velocityDeviation)
{
    mVelocityDeviation = velocityDeviation;
}

void SimpleVelocityModule::OnEmitParticles()
{
    const float signX = (Random::Float() * 2.0f) - 1.0f;
    const float signY = (Random::Float() * 2.0f) - 1.0f;
    const float signZ = (Random::Float() * 2.0f) - 1.0f;
    mCurrentSpawnVelocityDeviation = glm::vec3(signX, signY, signZ) * mVelocityDeviation;
}

void SimpleVelocityModule::SetExtraVelocityPower(const float velocityPower)
{
    mExtraVelocityPower = velocityPower;
}
} // namespace EngineCore