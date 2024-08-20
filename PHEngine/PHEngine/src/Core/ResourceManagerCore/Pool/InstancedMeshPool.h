#pragma once

#include <string>

#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/MeshAllocationPolicy.h"

using namespace Graphics::Mesh;

namespace Resources
{

	class InstancedMeshPool : public PoolBase<Skin, MeshPoolParameters, MeshAllocationPolicy>
	{
		static std::unique_ptr<InstancedMeshPool> m_instance;

	public:

		using poolType_t = PoolBase<Skin, MeshPoolParameters, MeshAllocationPolicy>;

		std::string ToString() const override;

		static std::unique_ptr<InstancedMeshPool>& GetInstance();

		static void ReloadInstance();
	};

}

