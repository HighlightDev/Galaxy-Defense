#include "StreamingSoundBufferBundle.h"

#include "Core/AudioCore/ErrorHandler.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/ResourceManagerCore/Pool/SoundStreamPool.h"

#include <AL/alext.h>
#include <TinyLogger/LogInterface.h>
#include <malloc.h>

#include <cstddef>

using namespace Resources;
using namespace TinyLogger;

namespace EngineCore {

StreamingSoundBufferBundle::StreamingSoundBufferBundle(const std::string& soundName, const ALuint soundDescriptor)
    : mSoundDescriptor(soundDescriptor)
{
    Init(soundName);
}

StreamingSoundBufferBundle::~StreamingSoundBufferBundle()
{
    alSourceUnqueueBuffers(mSoundDescriptor, NUM_BUFFERS, mBuffers);
    alDeleteBuffers(NUM_BUFFERS, mBuffers);
    CleanUp();
}

void StreamingSoundBufferBundle::CleanUp()
{
    if (mSoundMemoryChunk) {
        SoundStreamPool::GetInstance()->TryToFreeMemory(mSoundMemoryChunk);
        mSoundMemoryChunk = nullptr;
    }
}

void StreamingSoundBufferBundle::Init(const std::string& soundName)
{
    mSoundMemoryChunk = SoundStreamPool::GetInstance()->GetOrAllocateResource(soundName);
    alGenBuffers(NUM_BUFFERS, mBuffers);
}

void StreamingSoundBufferBundle::PrePlayFillBuffers(const ALuint sourceDesc)
{
    const AudioResourceInfo& audioInfo = mSoundMemoryChunk->GetAudioInfo();

    /* Rewind the source position and clear the buffer queue */
    alCall(alSourceRewind, sourceDesc);
    alCall(alSourcei, sourceDesc, AL_BUFFER, 0);

    ALsizei queuedBufferIndex;
    mSoundMemoryChunk->ReadStreamFromStart();

    static constexpr size_t one_buffer_size = 65536UL;
    /* Fill the buffer queue */
    for (queuedBufferIndex = 0; queuedBufferIndex < NUM_BUFFERS; ++queuedBufferIndex) {
        const int32_t chunkSamplesCount = mSoundMemoryChunk->ReadNewDataPortionIntoChunk();
        if (chunkSamplesCount < 1)
            break;

        ALvoid* audioData = static_cast<ALvoid*>(mSoundMemoryChunk->GetCurrentDataChunk());
        const int32_t chunkBytesCount = (ALsizei)(chunkSamplesCount * audioInfo.mChannelsCount) * (ALsizei)sizeof(short);

        alCall(
            alBufferData,
            mBuffers[queuedBufferIndex],
            audioInfo.mAudioFormat,
            audioData,
            (ALsizei)chunkBytesCount,
            audioInfo.mSampleRate);
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

    const AudioResourceInfo& audioInfo = mSoundMemoryChunk->GetAudioInfo();
    static constexpr size_t one_buffer_size = 65536UL;

    /* Unqueue and handle each processed buffer */
    while (processed > 0) {
        ALuint bufId;

        alCall(alSourceUnqueueBuffers, sourceDesc, 1, &bufId);
        processed--;

        const int32_t chunkSamplesCount = mSoundMemoryChunk->ReadNewDataPortionIntoChunk();
        if (chunkSamplesCount < 1)
            continue;

        const ALvoid* audioData = static_cast<ALvoid*>(mSoundMemoryChunk->GetCurrentDataChunk());
        const int32_t chunkBytesCount = (ALsizei)(chunkSamplesCount * audioInfo.mChannelsCount) * (ALsizei)sizeof(short);

        alCall(alBufferData, bufId, audioInfo.mAudioFormat, audioData, (ALsizei)chunkBytesCount, audioInfo.mSampleRate);
        alCall(alSourceQueueBuffers, sourceDesc, 1, &bufId);
    }

    /* Make sure the source hasn't underrun */
    if (state != AL_PLAYING && state != AL_PAUSED) {
        ALint queued;
        alCall(alGetSourcei, sourceDesc, AL_BUFFERS_QUEUED, &queued);
        if (0 == queued) {
            mHasQueuedBuffers = false;
        }
    }
}

} // namespace EngineCore
