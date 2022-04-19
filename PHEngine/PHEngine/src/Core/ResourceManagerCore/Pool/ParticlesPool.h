#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/ParticlesAllocationPolicy.h"
#include "Core/GameCore/Particles/ParticlePoolParameters.h"

using namespace EngineCore;

namespace Resources
{
   class ParticlesPool :
      public PoolBase<Skin, ParticlePoolParameters, ParticlesAllocationPolicy>
   {
      static std::unique_ptr<ParticlesPool> m_instance;

   public:

      using poolType_t = PoolBase<Skin, ParticlePoolParameters, ParticlesAllocationPolicy>;

      static std::unique_ptr<ParticlesPool>& GetInstance()
      {
         if (!m_instance)
            m_instance = std::make_unique<ParticlesPool>();

         return m_instance;
      }

      static void ReloadInstance()
      {
         if (m_instance)
            m_instance.reset();
      }
   };

}
