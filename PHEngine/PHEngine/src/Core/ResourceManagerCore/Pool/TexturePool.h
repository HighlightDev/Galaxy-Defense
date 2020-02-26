#pragma once

#include <string>
#include <memory>

#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/Policy/TextureAllocationPolicy.h"

using namespace Graphics::Texture;

namespace Resources
{

	class TexturePool : public PoolBase<ITexture, std::string, TextureAllocationPolicy>
	{
		static std::unique_ptr<TexturePool> m_instance;

	public:

		using poolType_t = PoolBase<ITexture, std::string, TextureAllocationPolicy>;

		static std::unique_ptr<TexturePool>& GetInstance()
		{
			if (!m_instance)
				m_instance = std::move(std::make_unique<TexturePool>());

			return m_instance;
		}

		static void ReloadInstance()
		{
			if (m_instance)
				m_instance.reset();
		}
	};

}