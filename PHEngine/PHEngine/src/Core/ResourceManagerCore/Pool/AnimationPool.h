#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/Animation/AnimationSequence.h"
#include "Core/ResourceManagerCore/Policy/AnimationAllocationPolicy.h"

namespace Resources
{
   class AnimationPool : public PoolBase<std::vector<AnimationSequence>, std::string, AnimationAllocationPolicy>
   {
      static std::unique_ptr<AnimationPool> m_instance;

   public:

      using poolType_t = PoolBase<std::vector<AnimationSequence>, std::string, AnimationAllocationPolicy>;

      static std::unique_ptr<AnimationPool>& GetInstance()
      {
         if (!m_instance)
            m_instance = std::make_unique<AnimationPool>();

         return m_instance;
      }

      static void ReloadInstance()
      {
         if (m_instance)
            m_instance.reset();
      }
   };

}
