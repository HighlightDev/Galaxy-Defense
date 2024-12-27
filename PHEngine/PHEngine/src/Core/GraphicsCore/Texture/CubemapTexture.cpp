#include "CubemapTexture.h"
#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"
#include "Core/IoCore/RawResource.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;
using namespace IO;

namespace Graphics
{
	namespace Texture
	{

		CubemapTexture::CubemapTexture(const std::vector<std::string> &pathToTextures)
		{
			m_texDescriptor = CreateCubemapTexture(pathToTextures);
			LogInfo( "CubemapTexture::ctor(const std::vector<std::string> &) => m_texDescriptor = ", m_texDescriptor);
		}

		CubemapTexture::CubemapTexture(TexParams cubemapTexParams)
			: m_texParams({cubemapTexParams, cubemapTexParams, cubemapTexParams, cubemapTexParams, cubemapTexParams, cubemapTexParams})
		{
			m_texDescriptor = CreateEmptyCubemapTexture();
			LogInfo( "CubemapTexture::ctor(TexParams) => m_texDescriptor = ", m_texDescriptor);
		}

		CubemapTexture::~CubemapTexture()
		{
			LogInfo( "CubemapTexture::dctor");
		}

		uint32_t CubemapTexture::CreateEmptyCubemapTexture()
		{
			uint32_t resultTextureDescriptor = -1;
			assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));

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

		uint32_t CubemapTexture::CreateCubemapTexture(const std::vector<std::string> &pathToTextures)
		{
			uint32_t resultTextureDescriptor = -1;
			size_t mutualPixelFormat = -1;
			assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));

			glGenTextures(1, &resultTextureDescriptor);
			glBindTexture(GL_TEXTURE_CUBE_MAP, resultTextureDescriptor);

			const size_t texturesCount = pathToTextures.size();
			for (size_t texIndex = 0; texIndex < texturesCount; texIndex++)
			{
				TexParams texParam;

				Resource *outResource;
				bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, pathToTextures[texIndex]);

				assert(bResourceValid);

				TextureResource *texResource = static_cast<TextureResource *>(outResource);

				texParam.TexBufferWidth = texResource->mTexInfo.Width;
				texParam.TexBufferHeight = texResource->mTexInfo.Height;

				if (texResource->mTexInfo.PixelComponents == 3)
				{
					texParam.TexPixelFormat = GL_RGB;
					texParam.TexPixelInternalFormat = GL_RGB;
				}
				else if (texResource->mTexInfo.PixelComponents == 4)
				{
					texParam.TexPixelFormat = GL_RGBA;
					texParam.TexPixelInternalFormat = GL_RGBA;
				}

				if (mutualPixelFormat == -1)
				{
					mutualPixelFormat = texParam.TexPixelFormat;
				}
				else if (mutualPixelFormat != texParam.TexPixelFormat)
				{
					throw std::invalid_argument("Every texture must have same pixel format.");
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + texIndex, 0, texParam.TexPixelInternalFormat, texParam.TexBufferWidth, texParam.TexBufferHeight, 0, texParam.TexPixelFormat, texParam.TexPixelType, texResource->mData);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texParam.TexMagFilter);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texParam.TexMinFilter);

				m_texParams.emplace_back(std::move(texParam));
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
			LogInfo( "CubemapTexture::CleanUp => m_texDescriptor = ", m_texDescriptor);
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

		eTextureType CubemapTexture::GetTextureType() const
		{
			return eTextureType::TEXTURE_CUBE;
		}
	}
}
