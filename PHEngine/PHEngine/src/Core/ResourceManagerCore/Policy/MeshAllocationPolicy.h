#pragma once
#include <string>
#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"

using namespace Graphics::Mesh;

namespace Resources
{
	class MeshAllocationPolicy
	{
	public:

		static std::shared_ptr<Skin> AllocateMemory(const std::string& arg);

		static void DeallocateMemory(std::shared_ptr<Skin> arg);
	};

}

