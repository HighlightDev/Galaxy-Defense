#pragma once
#include <string>
#include <memory>
#include <cstddef>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/SimplePrimitivePoolParameters.h"

using namespace Graphics::Mesh;

namespace Resources
{
   class SimplePrimitiveAllocationPolicy
   {
   public:

      static std::shared_ptr<Skin> AllocateMemory(const SimplePrimitivePoolParameters& arg);

      static void DeallocateMemory(const std::shared_ptr<Skin>& arg);
   };

}

