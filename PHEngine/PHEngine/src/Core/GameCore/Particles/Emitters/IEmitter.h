#pragma once

#include <memory>
#include <vector>
#include <glm/vec4.hpp>

#include "Core/GameCore/Particles/Particle.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"

namespace EngineCore
{
   class ParticleSystemComponent;

   class IEmitter
   {
   protected:

      std::weak_ptr<ParticleSystemComponent> mOwner;

      glm::vec4 mColorBegin;

      glm::vec4 mColorEnd;

      float mSizeBegin;

      float mSizeEnd;

      float mLifeTime;

   public:

      IEmitter();

      void SetOwner(const std::weak_ptr<ParticleSystemComponent>& owner);

      void SetColor(const glm::vec4& colorBegin, const glm::vec4& colorEnd);

      void SetSize(const float sizeBegin, const float sizeEnd);

      void SetLifeTime(const float lifeTime);

      virtual void EmitParticles(const size_t particlesCount) = 0;

   protected:

      std::vector<Particle>& GetParticlesPool();
   };

}

