#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/RuntimeGeneratedMeshAllocationPolicy.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedMeshPoolParameters.h"

using namespace EngineCore;

namespace Resources
{
   class RuntimeGeneratedMeshPool :
      public PoolBase<Skin, RuntimeGeneratedMeshPoolParameters, RuntimeGeneratedMeshAllocationPolicy>
   {
      static std::unique_ptr<RuntimeGeneratedMeshPool> m_instance;

   public:

      using poolType_t = PoolBase<Skin, RuntimeGeneratedMeshPoolParameters, RuntimeGeneratedMeshAllocationPolicy>;

      std::string ToString() const override;

      static std::unique_ptr<RuntimeGeneratedMeshPool>& GetInstance();

      static void ReloadInstance();
   };

}
