#include "SimpleLifeTimeModule.h"

#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore {
SimpleLifeTimeModule::SimpleLifeTimeModule()
    : mLifeTime(1.0f)
{
}

void SimpleLifeTimeModule::Update(Particle& particle, const float deltaTime)
{
    particle.LifeRemaining -= deltaTime;
    if (particle.LifeRemaining <= 0.0f) {
        particle.isActive = false;
        particle.LifeRemaining = 0.0f;
    }
}

void SimpleLifeTimeModule::SetLifeTime(const float lifeTime)
{
    mLifeTime = lifeTime;
}

void SimpleLifeTimeModule::OnEmitParticles()
{
    auto& particles = GetParticlesPool();

    for (auto& particle : particles) {
        particle.LifeTime = mLifeTime;
        particle.LifeRemaining = mLifeTime;
        particle.isActive = true;
    }
}
} // namespace EngineCore