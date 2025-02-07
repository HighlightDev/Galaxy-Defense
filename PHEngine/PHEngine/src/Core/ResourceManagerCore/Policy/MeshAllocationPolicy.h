#pragma once
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"

#include <memory>
#include <string>

using namespace Graphics::Mesh;

namespace Resources {
class MeshAllocationPolicy {
public:
    static std::shared_ptr<Skin> AllocateMemory(const MeshPoolParameters& arg);

    static void DeallocateMemory(std::shared_ptr<Skin> arg);
};

} // namespace Resources
