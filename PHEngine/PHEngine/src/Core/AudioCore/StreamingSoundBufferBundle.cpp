#include "StreamingSoundBufferBundle.h"
#include "Core/ResourceManagerCore/Pool/SoundMemoryChunkPool.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/AudioCore/ErrorHandler.h"

#include <cstddef>
#include <AL/alext.h>
#include <malloc.h>
#include <TinyLogger/LogInterface.h>

using namespace Resources;
using namespace TinyLogger;

namespace EngineCore
{

	StreamingSoundBufferBundle::StreamingSoundBufferBundle(const std::string &soundName, const ALuint soundDescriptor)
		: mSoundDescriptor(soundDescriptor)
	{
		Init(soundName);
	}

	StreamingSoundBufferBundle::~StreamingSoundBufferBundle()
	{
		alCall(alSourceUnqueueBuffers, mSoundDescriptor, NUM_BUFFERS, mBuffers);
		alDeleteBuffers(NUM_BUFFERS, mBuffers);
		CleanUp();
	}

	void StreamingSoundBufferBundle::CleanUp()
	{
		if (mSoundMemoryChunk)
		{
			SoundMemoryChunkPool::GetInstance()->TryToFreeMemory(mSoundMemoryChunk);
			mSoundMemoryChunk = nullptr;
		}
	}

	void StreamingSoundBufferBundle::Init(const std::string &soundName)
	{
		mSoundMemoryChunk = SoundMemoryChunkPool::GetInstance()->GetOrAllocateResource(soundName);
		alGenBuffers(NUM_BUFFERS, mBuffers);
	}

	void StreamingSoundBufferBundle::PrePlayFillBuffers(const ALuint sourceDesc)
	{
		/* Rewind the source position and clear the buffer queue */
		alCall(alSourceRewind, sourceDesc);
		alCall(alSourcei, sourceDesc, AL_BUFFER, 0);

		ALsizei queuedBufferIndex;
		mBufferDataCursor = 0;
		const AudioResourceInfo &audioInfo = mSoundMemoryChunk->GetAudioInfo();
		short *audioData = mSoundMemoryChunk->GetData();
		size_t leftBytes = audioInfo.mNumBytes;
		const size_t frame_size = (BUFFER_SAMPLES * audioInfo.mChannelsCount) * sizeof(short);
		/* Fill the buffer queue */
		for (queuedBufferIndex = 0; queuedBufferIndex < NUM_BUFFERS; ++queuedBufferIndex)
		{
			if (leftBytes < 1)
				break;

			size_t currentBufferPortion = 0;
			if (leftBytes >= frame_size)
			{
				currentBufferPortion = frame_size;
				leftBytes -= frame_size;
			}
			else
			{
				currentBufferPortion = leftBytes;
				leftBytes = 0;
			}

			uint8_t *audioFileChunk = ((uint8_t *)audioData) + mBufferDataCursor;
			mBufferDataCursor += currentBufferPortion;

			alCall(alBufferData, mBuffers[queuedBufferIndex], audioInfo.mAudioFormat, audioFileChunk, (ALsizei)currentBufferPortion, audioInfo.mSampleRate);
		}

		alCall(alSourceQueueBuffers, sourceDesc, queuedBufferIndex, mBuffers);
		mHasQueuedBuffers = queuedBufferIndex > 0;
	}

	bool StreamingSoundBufferBundle::IsStreamingFinished() const
	{
		return !mHasQueuedBuffers;
	}

	void StreamingSoundBufferBundle::UpdateBufferStream(const ALuint sourceDesc)
	{
		if (IsStreamingFinished())
			return;

		ALint processed, state;

		/* Get relevant source info */
		alCall(alGetSourcei, sourceDesc, AL_SOURCE_STATE, &state);
		alCall(alGetSourcei, sourceDesc, AL_BUFFERS_PROCESSED, &processed);

		const AudioResourceInfo &audioInfo = mSoundMemoryChunk->GetAudioInfo();
		short *audioData = mSoundMemoryChunk->GetData();
		size_t leftBytes = audioInfo.mNumBytes - mBufferDataCursor;
		const size_t frame_size = (BUFFER_SAMPLES * audioInfo.mChannelsCount) * sizeof(short);

		/* Unqueue and handle each processed buffer */
		while (processed > 0)
		{
			ALuint bufId;

			alCall(alSourceUnqueueBuffers, sourceDesc, 1, &bufId);
			processed--;

			if (leftBytes < 1)
				continue;

			size_t currentBufferPortion = 0;
			if (leftBytes >= frame_size)
			{
				currentBufferPortion = frame_size;
				leftBytes -= frame_size;
			}
			else
			{
				currentBufferPortion = leftBytes;
				leftBytes = 0;
			}

			uint8_t *audioFileChunk = ((uint8_t *)audioData) + mBufferDataCursor;
			mBufferDataCursor += currentBufferPortion;

			alCall(alBufferData, bufId, audioInfo.mAudioFormat, audioFileChunk, (ALsizei)currentBufferPortion, audioInfo.mSampleRate);
			alCall(alSourceQueueBuffers, sourceDesc, 1, &bufId);
		}

		/* Make sure the source hasn't underrun */
		if (state != AL_PLAYING && state != AL_PAUSED)
		{
			ALint queued;
			alCall(alGetSourcei, sourceDesc, AL_BUFFERS_QUEUED, &queued);
			if (0 == queued)
			{
				mHasQueuedBuffers = false;
			}
		}
	}

}
