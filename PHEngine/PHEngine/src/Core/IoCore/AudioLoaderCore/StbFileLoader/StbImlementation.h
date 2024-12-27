#pragma once

#include <cstddef>
#include <string>
#include <memory>

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

struct stb_vorbis;

namespace IO
{
	namespace Audio
	{
		class StbSoundStream;

		class StbFileLoader
		{
			void *m_lastAllocatedMemory;

		public:
			StbFileLoader();

			~StbFileLoader();

			void *AllocateMemoryForAudioSource(const std::string &pathToFile, AudioResourceInfo &outAudioInfo);

			std::shared_ptr<StbSoundStream> OpenStream(const std::string &pathToFile, AudioResourceInfo &outAudioInfo);

			void ReleaseAudioMemory();
		};

		class StbSoundStream
		{
			::stb_vorbis *mStream;

			AudioResourceInfo mAudioInfo;

			short *mDataChunk{nullptr};

			int32_t mLastReadSamplesCount{0};

		public:
			explicit StbSoundStream(::stb_vorbis *stream, const AudioResourceInfo &audioInfo);

			~StbSoundStream();

			short *GetDataChunk() const;

			int32_t ReadNewDataPortionIntoChunk();

			int32_t GetLastReadDataSamplesCount() const;

			void ReadStreamFromStart();

			void CleanUp();
		};
	}
}
