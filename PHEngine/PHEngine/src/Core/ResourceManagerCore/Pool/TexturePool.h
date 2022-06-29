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

		virtual std::string ToString() const override;

		std::shared_ptr<ITexture> GetTextureAt(size_t index) const;

		static std::unique_ptr<TexturePool> &GetInstance();

		static void ReloadInstance();
	};

}