#include "IParticleModule.h"

#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"

namespace EngineCore {
IParticleModule::IParticleModule()
    : mOwner()
{
}

void IParticleModule::SetOwner(const std::weak_ptr<ParticleSystemBaseComponent>& owner)
{
    mOwner = owner;
}

std::vector<Particle>& IParticleModule::GetParticlesPool()
{
    const auto& ownerSp = mOwner.lock();
    ext_assert(ownerSp, "IParticleModule::GetParticlesPool: owner is null");
    return ownerSp->mParticlesPool;
}
} // namespace EngineCore