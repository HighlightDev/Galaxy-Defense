#include "SimpleSizeModule.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore
{
   SimpleSizeModule::SimpleSizeModule()
       : mSizeBegin(1.0f),
         mSizeEnd(1.0f)
   {
   }

   void SimpleSizeModule::Update(Particle &particle, const float deltaTime)
   {
      particle.Size = EngineMath::LerpFloat(particle.LifeTime - particle.LifeRemaining,
                                            0.0f,
                                            particle.LifeTime,
                                            mSizeBegin,
                                            mSizeEnd);
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
   }
}