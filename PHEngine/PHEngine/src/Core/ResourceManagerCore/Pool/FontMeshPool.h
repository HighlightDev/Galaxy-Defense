#pragma once

#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/GameCore/GUI/Common/TextMesh.h"
#include "Core/GameCore/GUI/Common/FontParams.h"
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

		std::string ToString() const override;

		static std::unique_ptr<FontMeshPool> &GetInstance();

		static void ReloadInstance();
	};

}
