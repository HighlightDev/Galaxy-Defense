#pragma once

#include <vector>
#include <string>

#include "ITexture.h"


namespace Graphics
{
	namespace Texture
	{

		class CubemapTexture :
			public ITexture
		{
			std::vector<TexParams> m_texParams;

		public:

			CubemapTexture(std::vector<std::string>& pathToTextures);

         CubemapTexture(TexParams cubemapTexParams);

			virtual ~CubemapTexture();

			virtual void BindTexture(uint32_t textureSlot) const override;

			virtual void UnbindTexture(uint32_t textureSlot) const override;

			virtual void CleanUp() override;

			virtual uint32_t GetTextureDescriptor() const override;

			virtual glm::ivec2 GetTextureRezolution() const override;

			virtual TexParams GetTextureParameters() const override;

         virtual float GetTextureAspectRatio() const override;

         virtual TextureType GetTextureType() const override;

		private:

			uint32_t CreateCubemapTexture(std::vector<std::string>& pathToTextures);

         uint32_t CreateEmptyCubemapTexture();
		};

	}
}

