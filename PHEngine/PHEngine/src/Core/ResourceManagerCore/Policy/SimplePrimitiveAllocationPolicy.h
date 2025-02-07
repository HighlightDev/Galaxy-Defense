#pragma once
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/SimplePrimitivePoolParameters.h"

#include <cstddef>
#include <memory>
#include <string>

using namespace Graphics::Mesh;

namespace Resources {
class SimplePrimitiveAllocationPolicy {
public:
    static std::shared_ptr<Skin> AllocateMemory(const SimplePrimitivePoolParameters& arg);

    static void DeallocateMemory(const std::shared_ptr<Skin>& arg);
};

} // namespace Resources
