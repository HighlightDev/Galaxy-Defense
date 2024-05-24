#pragma once
#include <string>
#include <memory>

#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"
#include "Core/GraphicsCore/Mesh/Skin.h"

using namespace Graphics::Mesh;

namespace Resources
{
	class MeshAllocationPolicy
	{
	public:

		static std::shared_ptr<Skin> AllocateMemory(const MeshPoolParameters& arg);

		static void DeallocateMemory(std::shared_ptr<Skin> arg);
	};

}

