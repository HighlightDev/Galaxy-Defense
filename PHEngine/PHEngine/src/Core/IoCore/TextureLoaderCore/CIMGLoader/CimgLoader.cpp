#include "CimgLoader.h"

namespace IO
{
	namespace Images
	{
		CimgLoader::CimgLoader()
			: m_lastAllocatedMemory(nullptr)
		{
		}

		CimgLoader::~CimgLoader()
		{
			ReleaseTextureMemory();
		}

		uint8_t *CimgLoader::AllocateTextureMemoryFromFile(const std::string &pathToFile, TextureResourceInfo &out_params)
		{
			return nullptr;
		}

		void CimgLoader::ReleaseTextureMemory()
		{
			if (m_lastAllocatedMemory)
			{
				m_lastAllocatedMemory = nullptr;
			}
		}
	}
}
