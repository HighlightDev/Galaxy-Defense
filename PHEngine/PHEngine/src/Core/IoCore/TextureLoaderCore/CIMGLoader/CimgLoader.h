#pragma once

#include <cstddef>
#include <string>

#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"

namespace IO
{
	namespace Images
	{
		class CimgLoader
		{

			uint8_t *m_lastAllocatedMemory;

		public:
			CimgLoader();

			~CimgLoader();

			uint8_t *AllocateTextureMemoryFromFile(const std::string &pathToFile, TextureResourceInfo &out_params);

			void ReleaseTextureMemory();
		};
	}
}
