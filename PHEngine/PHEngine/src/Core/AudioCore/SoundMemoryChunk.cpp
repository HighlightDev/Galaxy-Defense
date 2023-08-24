#include "SoundMemoryChunk.h"
#include "Core/CommonCore/Assertion.h"

#include <stdlib.h>

namespace EngineCore
{
    SoundMemoryChunk::SoundMemoryChunk(short *memoryChunkPtr, const AudioResourceInfo &audioInfo)
        : mSoundChunkData(memoryChunkPtr), mAudioInfo(audioInfo)
    {
    }

    short *SoundMemoryChunk::GetData() const
    {
        return mSoundChunkData;
    }

    bool SoundMemoryChunk::operator==(const SoundMemoryChunk &right) const
    {
        return this->mAudioInfo.mAudioFormat == right.mAudioInfo.mAudioFormat &&
               this->mAudioInfo.mChannelsCount == right.mAudioInfo.mChannelsCount &&
               this->mAudioInfo.mNumBytes == right.mAudioInfo.mNumBytes &&
               this->mAudioInfo.mSampleRate == right.mAudioInfo.mSampleRate;
    }

    const AudioResourceInfo &SoundMemoryChunk::GetAudioInfo() const
    {
        return mAudioInfo;
    }

    void SoundMemoryChunk::CleanUp()
    {
        assert(mSoundChunkData != nullptr);
        free(mSoundChunkData);
    }
}