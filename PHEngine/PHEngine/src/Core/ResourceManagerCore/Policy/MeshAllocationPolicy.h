#pragma once
#include <string>
#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"

using namespace Graphics::Mesh;

namespace Resources
{
   template <typename Model>
	class MeshAllocationPolicy
	{
	public:

		static std::shared_ptr<Skin> AllocateMemory(Model& arg);

		static void DeallocateMemory(std::shared_ptr<Skin> arg);
	};

}

