#pragma once

#include <stb/stb_image.h>
#include <stdint.h>
#include <string>
#include "Core/GraphicsCore/Texture/TexParams.h"

namespace IO
{
	namespace Images
	{
		namespace Stb
		{
			class StbLoader
			{

				uint8_t* m_lastAllocatedMemory;

			public:

            StbLoader();

            ~StbLoader();

				uint8_t* AllocateTextureMemoryFromFile(const std::string& pathToFile, Graphics::Texture::TexParams& out_params);

				void ReleaseTextureMemory();
   
			};
		}
	}
}
