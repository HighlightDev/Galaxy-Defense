#include "IEmitter.h"

#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"

namespace EngineCore {

IEmitter::IEmitter()
    : mOwner()
{
}

void IEmitter::SetOwner(const std::weak_ptr<CpuParticleSystemComponent>& owner)
{
    mOwner = owner;
}

std::vector<Particle>& IEmitter::GetParticlesPool()
{
    const auto& ownerSp = mOwner.lock();
    ext_assert(ownerSp, "IEmitter::GetParticlesPool: ownerSp is null");
    return ownerSp->mParticlesPool;
}

} // namespace EngineCore
