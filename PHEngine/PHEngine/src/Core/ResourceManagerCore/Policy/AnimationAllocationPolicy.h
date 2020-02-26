#pragma once

#include <memory>
#include <vector>

#include "Core/GraphicsCore/Animation/AnimationSequence.h"

using namespace Graphics::Animation;

namespace Resources
{

   template <typename Model>
   class AnimationAllocationPolicy
   {
   public:

      AnimationAllocationPolicy() = default;

      ~AnimationAllocationPolicy() = default;

      static std::shared_ptr<std::vector<AnimationSequence>> AllocateMemory(Model& arg);

      static void DeallocateMemory(std::shared_ptr<std::vector<AnimationSequence>> arg);
   };

}
