#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/SimplePrimitiveAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/SimplePrimitivePoolParameters.h"

namespace Resources
{

   class SimplePrimitivePool : public PoolBase<Skin, SimplePrimitivePoolParameters, SimplePrimitiveAllocationPolicy>
   {
      static std::unique_ptr<SimplePrimitivePool> m_instance;

   public:
      using poolType_t = PoolBase<Skin, SimplePrimitivePoolParameters, SimplePrimitiveAllocationPolicy>;

      std::string ToString() const override;

      static std::unique_ptr<SimplePrimitivePool> &GetInstance();

      static void ReloadInstance();
   };

}
