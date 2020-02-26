#pragma once

#include "ITexture.h"
#include "TextureMipMapState.h"

#include <string>
#include <gl/glew.h>

namespace Graphics
{
	namespace Texture
	{

		class Texture2d :
			public ITexture
		{
		private:

			TexParams m_textureParams;
			
			ITextureMipMapState* m_mipmapState;

		public:

			Texture2d(std::string& pathToTex, ITextureMipMapState* mipmapState);

			Texture2d(uint32_t texDescriptor, glm::ivec2 texBufferWH);

         Texture2d(TexParams&& textureParameters);

         Texture2d(TexParams& textureParameters);

			virtual ~Texture2d();

         virtual void BindTexture(uint32_t textureSlot) const override;

         virtual void UnbindTexture(uint32_t textureSlot) const override;

			void CleanUp();

			inline uint32_t GetTextureDescriptor() const {

            return m_texDescriptor;
         }

			inline glm::ivec2 GetTextureRezolution() const {

				return glm::ivec2(m_textureParams.TexBufferWidth, m_textureParams.TexBufferHeight);
			}

			inline TexParams GetTextureParameters() const {

				return m_textureParams;
			}

         virtual float GetTextureAspectRatio() const override;

         virtual TextureType GetTextureType() const override;

		private:

         void InitEmptyTexture();

			uint32_t LoadTextureFromFile(std::string& pathToTex, int32_t texWrapMode = GL_REPEAT);

			uint32_t CreateTexture(const void* pixelsData);
		};

	}
}

