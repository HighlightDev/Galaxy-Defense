#include "SimpleLifeTimeModule.h"

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/Lifetime/SimpleLifeTimeModuleGpuProxy.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore {
SimpleLifeTimeModule::SimpleLifeTimeModule()
    : mLifeTime(1.0f)
{
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

void SimpleLifeTimeModule::EmitSingleParticle(Particle& particle)
{
    particle.LifeTime = mLifeTime;
    particle.LifeRemaining = mLifeTime;
    particle.isActive = true;
}

std::shared_ptr<IGpuParticleModuleProxy> SimpleLifeTimeModule::GetGpuProxy() const
{
    return std::make_shared<SimpleLifeTimeModuleGpuProxy>(mLifeTime);
}
} // namespace EngineCore