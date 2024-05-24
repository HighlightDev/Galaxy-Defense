#pragma once
#include <string>
#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"

using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources
{
	class RuntimeGeneratedMeshAllocationPolicy
	{
	public:
		static std::shared_ptr<Skin> AllocateMemory(const RuntimeGeneratedMeshPoolParameters &arg);

		static void DeallocateMemory(std::shared_ptr<Skin> arg);
	};
}
