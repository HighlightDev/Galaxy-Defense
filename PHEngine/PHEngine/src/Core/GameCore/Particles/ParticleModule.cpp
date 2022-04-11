#include "ParticleModule.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"

namespace EngineCore
{
    ParticleModule::ParticleModule(const std::weak_ptr<ParticleSystemComponent> &owner)
        : mOwner(owner)
    {
    }
}