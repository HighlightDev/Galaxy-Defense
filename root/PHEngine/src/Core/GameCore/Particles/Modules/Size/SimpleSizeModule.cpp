#include "SimpleSizeModule.h"

#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/Size/SimpleSizeModuleGpuProxy.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore {
SimpleSizeModule::SimpleSizeModule()
    : mSizeBegin(1.0f)
    , mSizeEnd(1.0f)
{
}

void SimpleSizeModule::Update(Particle& particle, const float deltaTimeSec)
{
    particle.Size
        = EngineMath::LerpFloat(particle.LifeTime - particle.LifeRemaining, 0.0f, particle.LifeTime, mSizeBegin, mSizeEnd);
}

void SimpleSizeModule::SetSizeBegin(const float sizeBegin)
{
    mSizeBegin = sizeBegin;
}

void SimpleSizeModule::SetSizeEnd(const float sizeEnd)
{
    mSizeEnd = sizeEnd;
}

void SimpleSizeModule::OnEmitParticles()
{
    auto& particles = GetParticlesPool();
    for (auto& particle : particles) {
        particle.Size = mSizeBegin;
    }
}

void SimpleSizeModule::EmitSingleParticle(Particle& particle)
{
    particle.Size = mSizeBegin;
}

std::shared_ptr<IGpuParticleModuleProxy> SimpleSizeModule::GetGpuProxy() const
{
    return std::make_shared<SimpleSizeModuleGpuProxy>(mSizeBegin, mSizeEnd);
}
} // namespace EngineCore