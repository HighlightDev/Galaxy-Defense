#include "CubemapTexture.h"
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"

using namespace Io::Images::Stb;

namespace Graphics
{
	namespace Texture
	{

		CubemapTexture::CubemapTexture(std::vector<std::string>& pathToTextures)
		{
			m_texDescriptor = CreateCubemapTexture(pathToTextures);
		}

      CubemapTexture::CubemapTexture(TexParams cubemapTexParams)
         : m_texParams({ cubemapTexParams, cubemapTexParams, cubemapTexParams, cubemapTexParams, cubemapTexParams, cubemapTexParams })
      {
         m_texDescriptor = CreateEmptyCubemapTexture();
      }

		CubemapTexture::~CubemapTexture()
		{
		}

      uint32_t CubemapTexture::CreateEmptyCubemapTexture()
      {
         uint32_t resultTextureDescriptor = -1;

         glGenTextures(1, &resultTextureDescriptor);
         glBindTexture(GL_TEXTURE_CUBE_MAP, resultTextureDescriptor);

         for (size_t texIndex = 0; texIndex < m_texParams.size(); texIndex++)
         {
            TexParams texParam = m_texParams[texIndex];
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + texIndex, 0, texParam.TexPixelInternalFormat, texParam.TexBufferWidth, texParam.TexBufferHeight, 0, texParam.TexPixelFormat, texParam.TexPixelType, NULL);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texParam.TexMagFilter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texParam.TexMinFilter);
         }

         glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

         return resultTextureDescriptor;
      }

		uint32_t CubemapTexture::CreateCubemapTexture(std::vector<std::string>& pathToTextures)
		{
			uint32_t resultTextureDescriptor = -1;
			size_t mutualPixelFormat = -1;

			glGenTextures(1, &resultTextureDescriptor);
			glBindTexture(GL_TEXTURE_CUBE_MAP, resultTextureDescriptor);

			const size_t texturesCount = pathToTextures.size();
			for (size_t texIndex = 0; texIndex < texturesCount; texIndex++)
			{
				TexParams texParam;
				uint8_t* texData = StbLoader::GetInstance().AllocateTextureMemoryFromFile(pathToTextures[texIndex], texParam);
				if (mutualPixelFormat == -1)
				{
					mutualPixelFormat = texParam.TexPixelFormat;
				}
				else if (mutualPixelFormat != texParam.TexPixelFormat)
				{
					throw std::invalid_argument("Every texture must have same pixel format.");
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + texIndex, 0, texParam.TexPixelInternalFormat, texParam.TexBufferWidth, texParam.TexBufferHeight, 0, texParam.TexPixelFormat, texParam.TexPixelType, texData);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texParam.TexMagFilter);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texParam.TexMinFilter);

				m_texParams.emplace_back(std::move(texParam));

				StbLoader::GetInstance().ReleaseTextureMemory();
			}

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			return resultTextureDescriptor;
		}

		void CubemapTexture::BindTexture(uint32_t textureSlot) const
		{
			glActiveTexture(GL_TEXTURE0 + textureSlot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_texDescriptor);
		}

		void CubemapTexture::UnbindTexture(uint32_t textureSlot) const
		{
			glActiveTexture(GL_TEXTURE0 + textureSlot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		void CubemapTexture::CleanUp()
		{
			glDeleteTextures(1, &m_texDescriptor);
		}

		uint32_t CubemapTexture::GetTextureDescriptor() const
		{
			return m_texDescriptor;
		}

		glm::ivec2 CubemapTexture::GetTextureRezolution() const
		{
			return glm::ivec2(m_texParams[0].TexBufferWidth, m_texParams[0].TexBufferHeight);
		}

		TexParams CubemapTexture::GetTextureParameters() const
		{
			return m_texParams[0];
		}

      float CubemapTexture::GetTextureAspectRatio() const
      {
         return (static_cast<float>(m_texParams[0].TexBufferWidth) / static_cast<float>(m_texParams[0].TexBufferHeight));
      }

      TextureType CubemapTexture::GetTextureType() const
      {
         return TextureType::TEXTURE_CUBE;
      }
	}
}
