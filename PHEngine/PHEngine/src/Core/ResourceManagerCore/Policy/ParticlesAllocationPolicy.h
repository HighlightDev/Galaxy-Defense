#pragma once
#include <string>
#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GameCore/Particles/ParticlePoolParameters.h"

using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources
{
	class ParticlesAllocationPolicy
	{
	public:

		static std::shared_ptr<Skin> AllocateMemory(const ParticlePoolParameters& arg);

		static void DeallocateMemory(std::shared_ptr<Skin> arg);
	};
}

