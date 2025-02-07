#pragma once
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"

#include <memory>
#include <string>

using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources {
class ParticlesAllocationPolicy {
public:
    static std::shared_ptr<Skin> AllocateMemory(const ParticlePoolParameters& arg);

    static void DeallocateMemory(std::shared_ptr<Skin> arg);
};
} // namespace Resources
