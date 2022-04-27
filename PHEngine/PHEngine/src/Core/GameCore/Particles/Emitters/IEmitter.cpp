#include "IEmitter.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"

namespace EngineCore
{

   IEmitter::IEmitter()
      : mOwner(),
      mColorBegin(),
      mColorEnd(),
      mSizeBegin(1.0f),
      mLifeTime(1.0f)
   {
   }

   void IEmitter::SetOwner(const std::weak_ptr<ParticleSystemComponent>& owner)
   {
      mOwner = owner;
   }

   void IEmitter::SetColor(const glm::vec4& colorBegin, const glm::vec4& colorEnd)
   {
      mColorBegin = colorBegin;
      mColorEnd = colorEnd;
   }

   void IEmitter::SetSize(const float sizeBegin, const float sizeEnd)
   {
      mSizeBegin = sizeBegin;
      mSizeEnd = sizeEnd;
   }

   void IEmitter::SetLifeTime(const float lifeTime)
   {
      mLifeTime = lifeTime;
   }

   std::vector<Particle>& IEmitter::GetParticlesPool()
   {
      const auto& ownerSp = mOwner.lock();
      assert(ownerSp);
      return ownerSp->mParticlesPool;
   }

}
