#include "SoundMemoryChunk.h"
#include "Core/CommonCore/Assertion.h"

#include <stdlib.h>

namespace EngineCore
{
    SoundMemoryChunk::SoundMemoryChunk(short *memoryChunkPtr, const AudioResourceInfo &audioInfo)
    : mSoundChunkData(memoryChunkPtr)
    , mAudioInfo(audioInfo)
    {
    }

    short *SoundMemoryChunk::GetData() const
    {
        return mSoundChunkData;
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