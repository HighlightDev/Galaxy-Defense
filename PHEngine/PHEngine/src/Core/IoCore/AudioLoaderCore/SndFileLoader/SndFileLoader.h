#pragma once

#include <cstddef>
#include <string>

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

namespace IO
{
	namespace Audio
	{
		class SndFileLoader
		{
			void *m_lastAllocatedMemory;

		public:
			SndFileLoader();

			~SndFileLoader();

			void *AllocateMemoryForAudioSource(const std::string &pathToFile, AudioResourceInfo& outAudioInfo);

			void ReleaseAudioMemory();
		};
	}
}
