#include "SimpleVelocityModule.h"

#include "Core/CommonCore/Random.h"

namespace EngineCore
{
   SimpleVelocityModule::SimpleVelocityModule()
      : mVelocityDirection()
      , mVelocityDeviation()
      , mCurrentSpawnVelocityDeviation()
   {
   }

   void SimpleVelocityModule::Tick(const float deltaTime)
   {
      auto& particlesPool = GetParticlesPool();
      
      const size_t particlesCount = particlesPool.size();

      for (size_t i = 0; i < particlesCount; ++i)
      {
         Particle& particle = particlesPool[i];
        
         particle.Velocity = (mVelocityDirection * deltaTime) + (mCurrentSpawnVelocityDeviation * deltaTime);
      }
   }

   void SimpleVelocityModule::SetVelocityDirection(const glm::vec3& velocityDirection)
   {
      mVelocityDirection = velocityDirection;
   }

   void SimpleVelocityModule::SetVelocityDeviation(const glm::vec3& velocityDeviation) 
   {
      mVelocityDeviation = velocityDeviation;
   }

   void SimpleVelocityModule::OnEmitParticles()
   {
      const float signX = (Random::Float() * 2.0f) - 1.0f;
      const float signY = (Random::Float() * 2.0f) - 1.0f;
      const float signZ = (Random::Float() * 2.0f) - 1.0f;
      mCurrentSpawnVelocityDeviation = glm::vec3(signX, signY, signZ) * mVelocityDeviation;
   }
}