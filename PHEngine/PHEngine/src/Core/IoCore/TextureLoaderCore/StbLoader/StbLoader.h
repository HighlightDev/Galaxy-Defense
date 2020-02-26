#pragma once

#include <stb/stb_image.h>
#include <stdint.h>
#include <string>
#include "Core/GraphicsCore/Texture/TexParams.h"

namespace Io
{
	namespace Images
	{
		namespace Stb
		{
			class StbLoader
			{

				static StbLoader* m_instance;

				uint8_t* m_lastAllocatedMemory;

				StbLoader();

			public:

				static StbLoader& GetInstance()
				{
					if (!m_instance)
						m_instance = new StbLoader();

					return *m_instance;
				}

				static void ReleaseInstance()
				{
					if (m_instance)
						delete m_instance;
				}

				uint8_t* AllocateTextureMemoryFromFile(std::string& pathToFile, Graphics::Texture::TexParams& out_params);

				void ReleaseTextureMemory();

				~StbLoader();
			};
		}
	}
}
