#include "IEmitter.h"

#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"

namespace EngineCore {

IEmitter::IEmitter()
    : mOwner()
{
}

void IEmitter::SetOwner(const std::weak_ptr<ParticleSystemComponent>& owner)
{
    mOwner = owner;
}

std::vector<Particle>& IEmitter::GetParticlesPool()
{
    const auto& ownerSp = mOwner.lock();
    assert(ownerSp);
    return ownerSp->mParticlesPool;
}

} // namespace EngineCore
