#define STB_IMAGE_IMPLEMENTATION
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"

namespace IO
{
	namespace Images
	{
		namespace Stb
		{

			StbLoader::StbLoader()
				: m_lastAllocatedMemory(nullptr)
			{
			}

			StbLoader::~StbLoader()
			{
				ReleaseTextureMemory();
			}

			uint8_t* StbLoader::AllocateTextureMemoryFromFile(const std::string& pathToFile, TextureResourceInfo& out_params)
			{
				int32_t width, height, components;

				ReleaseTextureMemory(); // If memory is already possessed by other texture, first of all 
										// release previously allocated memory

				uint8_t* texData = stbi_load(pathToFile.c_str(), &width, &height, &components, STBI_rgb_alpha);

				if (texData != nullptr)
					m_lastAllocatedMemory = texData;

				out_params.Width = width;
				out_params.Height = height;
				out_params.PixelComponents = components;

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
