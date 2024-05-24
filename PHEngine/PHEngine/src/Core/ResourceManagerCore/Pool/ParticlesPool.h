#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/ParticlesAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"

using namespace EngineCore;

namespace Resources
{
   class ParticlesPool :
      public PoolBase<Skin, ParticlePoolParameters, ParticlesAllocationPolicy>
   {
      static std::unique_ptr<ParticlesPool> m_instance;

   public:

      using poolType_t = PoolBase<Skin, ParticlePoolParameters, ParticlesAllocationPolicy>;

      std::string ToString() const override;

      static std::unique_ptr<ParticlesPool>& GetInstance();

      static void ReloadInstance();
   };

}
