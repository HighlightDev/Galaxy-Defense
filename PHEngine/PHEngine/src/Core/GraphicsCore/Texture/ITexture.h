#pragma once

#include <stdint.h>
#include <glm/vec2.hpp>

#include "TexParams.h"

namespace Graphics
{
	namespace Texture
	{
      enum class TextureType
      {
         UNDEFINED,
         TEXTURE_2D,
         TEXTURE_CUBE
      };

		class ITexture
		{
		protected:

			uint32_t m_texDescriptor;

		public:
			ITexture();

			virtual ~ITexture();

			virtual void BindTexture(uint32_t textureSlot) const = 0;
			virtual void UnbindTexture(uint32_t textureSlot) const = 0;
			virtual void CleanUp() = 0;
			virtual uint32_t GetTextureDescriptor() const = 0;
			virtual glm::ivec2 GetTextureRezolution() const = 0;
			virtual TexParams GetTextureParameters() const = 0;
         virtual float GetTextureAspectRatio() const = 0;
         virtual TextureType GetTextureType() const = 0;
		};
	}
}
