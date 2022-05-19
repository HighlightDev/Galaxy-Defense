#pragma once

#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/GameCore/GUI/Text/TextMesh.h"
#include "Core/GameCore/GUI/Text/FontParams.h"
#include "Core/ResourceManagerCore/Policy/FontMeshAllocationPolicy.h"

using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources
{
	class FontMeshPool : public PoolBase<TextMesh, FontParams, FontMeshAllocationPolicy>
	{
		static std::unique_ptr<FontMeshPool> m_instance;

	public:

		using poolType_t = PoolBase<TextMesh, FontParams, FontMeshAllocationPolicy>;

		static std::unique_ptr<FontMeshPool>& GetInstance()
		{
			if (!m_instance)
				m_instance = std::make_unique<FontMeshPool>();

			return m_instance;
		}

		static void ReloadInstance()
		{
			if (m_instance)
				m_instance.reset();
		}
	};

}

