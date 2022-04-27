#include "IParticleModule.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"

namespace EngineCore
{
   IParticleModule::IParticleModule()
        : mOwner()
    {
    }

   void IParticleModule::SetOwner(const std::weak_ptr<ParticleSystemComponent>& owner)
   {
      mOwner = owner;
   }

   std::vector<Particle>& IParticleModule::GetParticlesPool()
   {
      const auto& ownerSp = mOwner.lock();
      assert(ownerSp);
      return ownerSp->mParticlesPool;
   }
}