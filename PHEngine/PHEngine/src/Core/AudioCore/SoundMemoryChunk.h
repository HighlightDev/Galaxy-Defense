#pragma once

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

using namespace IO::Audio;

namespace EngineCore
{
    class SoundMemoryChunk
    {
        short* mSoundChunkData;

        AudioResourceInfo mAudioInfo;

        public:

        SoundMemoryChunk(short* memoryChunkPtr, const AudioResourceInfo& audioInfo);

        bool operator==(const SoundMemoryChunk& right) const;

        short* GetData() const;

        const AudioResourceInfo& GetAudioInfo() const;

        void CleanUp();
        
    };
}