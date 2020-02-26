#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/SimplePrimitiveAllocationPolicy.h"

namespace Resources
{

   class SimplePrimitivePool :
      public PoolBase<Skin, int32_t, SimplePrimitiveAllocationPolicy>
   {
      static std::unique_ptr<SimplePrimitivePool> m_instance;

   public:

      using poolType_t = PoolBase<Skin, int32_t, SimplePrimitiveAllocationPolicy>;

      static std::unique_ptr<SimplePrimitivePool>& GetInstance()
      {
         if (!m_instance)
            m_instance = std::make_unique<SimplePrimitivePool>();

         return m_instance;
      }

      static void ReloadInstance()
      {
         if (m_instance)
            m_instance.reset();
      }
   };

}
