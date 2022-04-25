#include "ParticleExplosionEmitter.h"

#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore
{
   void ParticleExplosionEmitter::EmitParticles(const size_t particlesCount)
   {
      const auto& ownerSp = mOwner.lock();
      assert(ownerSp);

      auto& particlePool = GetParticlesPool();
      const auto& poolSize = particlePool.size();

      const size_t particlesEmitCount = particlesCount <= poolSize ? particlesCount == 0 ? poolSize : particlesCount : poolSize;
      
      static constexpr float radius = 20.0f;

      for (size_t i = 0; i < particlesEmitCount; ++i)
      {
         const float random_radius = 1.0f;
         const float random_theta_rad = Random::Float() * EngineMath::PI * 2;
         const float random_phi_rad = Random::Float() * EngineMath::PI;

         Particle &p = particlePool[i];

         p.Position = glm::vec3(
            random_radius * std::cos(random_theta_rad) * std::sin(random_phi_rad),
            random_radius * std::sin(random_theta_rad) * std::sin(random_theta_rad),
            random_radius * std::cos(random_phi_rad));
         p.Rotation = Random::Float() * EngineMath::PI * 2;

         const auto randomNormalizedValue = []()
         {
            return (Random::Float() * 2.0f) - 1.0f;
         };

         p.Velocity = glm::vec3(randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f);

         p.ColorBegin = mColorBegin;
         p.ColorEnd = mColorEnd;
         p.SizeBegin = mSizeBegin;
         p.SizeEnd = mSizeEnd;
         p.LifeTime = mLifeTime;
         p.LifeRemaining = p.LifeTime;

         p.isActive = true;
      }
   }
}
