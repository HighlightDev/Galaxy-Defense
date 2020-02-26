#pragma once

#include <string>

#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/MeshAllocationPolicy.h"

using namespace Graphics::Mesh;

namespace Resources
{

	class MeshPool : public PoolBase<Skin, std::string, MeshAllocationPolicy>
	{
		static std::unique_ptr<MeshPool> m_instance;

	public:

		using poolType_t = PoolBase<Skin, std::string, MeshAllocationPolicy>;

		static std::unique_ptr<MeshPool>& GetInstance()
		{
			if (!m_instance)
				m_instance = std::make_unique<MeshPool>();

			return m_instance;
		}

		static void ReloadInstance()
		{
			if (m_instance)
				m_instance.reset();
		}
	};

}

