#define STB_IMAGE_IMPLEMENTATION
#include "StbLoader.h"

namespace Io
{
	namespace Images
	{
		namespace Stb
		{

			StbLoader* StbLoader::m_instance = nullptr;

			StbLoader::StbLoader()
				: m_lastAllocatedMemory(nullptr)
			{
			}

			StbLoader::~StbLoader()
			{
				ReleaseTextureMemory();
			}

			uint8_t* StbLoader::AllocateTextureMemoryFromFile(std::string& pathToFile, Graphics::Texture::TexParams& out_params)
			{
				int32_t width, height, components;

				ReleaseTextureMemory(); // If memory is already possessed by other texture, first of all 
										// release previously allocated memory

				uint8_t* texData = stbi_load(pathToFile.c_str(), &width, &height, &components, STBI_rgb_alpha);

				if (texData != nullptr)
					m_lastAllocatedMemory = texData;

				out_params.TexBufferWidth = width;
				out_params.TexBufferHeight = height;

				if (components == 3)
				{
					out_params.TexPixelFormat = GL_RGB;
					out_params.TexPixelInternalFormat = GL_RGB;
				}
				else if (components == 4)
				{
					out_params.TexPixelFormat = GL_RGBA;
					out_params.TexPixelInternalFormat = GL_RGBA;
				}

				return texData;
			}

			void StbLoader::ReleaseTextureMemory()
			{
				if (m_lastAllocatedMemory)
				{
					stbi_image_free(m_lastAllocatedMemory);
					m_lastAllocatedMemory = nullptr;
				}
			}
		}
	}
}
