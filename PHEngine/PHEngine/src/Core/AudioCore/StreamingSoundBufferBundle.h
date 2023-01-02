#pragma once
#include <AL/al.h>
#include <sndfile/sndfile.h>
#include <memory>
#include <string>

#include "Core/AudioCore/SoundMemoryChunk.h"

namespace EngineCore
{

	class StreamingSoundBufferBundle
	{
		static constexpr size_t BUFFER_SAMPLES = 8192;

		static constexpr size_t NUM_BUFFERS = 4;

		ALuint mBuffers[NUM_BUFFERS];

		std::shared_ptr<SoundMemoryChunk> mSoundMemoryChunk;

		size_t mBufferDataCursor = 0;

		bool mHasQueuedBuffers = false;

	public:
		StreamingSoundBufferBundle(const std::string &soundName);

		~StreamingSoundBufferBundle();

		void PrePlayFillBuffers(const ALuint sourceDesc);

		void UpdateBufferStream(const ALuint sourceDesc);

		bool IsStreamingFinished() const;

	private:

		void Init(const std::string &soundName);
	};
}