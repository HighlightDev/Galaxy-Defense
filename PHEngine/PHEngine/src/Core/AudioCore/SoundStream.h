#pragma once

#include <memory>

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

using namespace IO::Audio;

namespace IO::Audio
{
    class StbSoundStream;
}

namespace EngineCore
{
    class SoundStream
    {
        std::shared_ptr<StbSoundStream> mStream;

        AudioResourceInfo mAudioInfo;

    public:
        SoundStream(const std::shared_ptr<StbSoundStream> &stream, const AudioResourceInfo &audioInfo);

        bool operator==(const SoundStream &right) const;

        short *GetCurrentDataChunk() const;

        int32_t ReadNewDataPortionIntoChunk();

        int32_t GetLastReadDataSamplesCount() const;

        void ReadStreamFromStart() const;

        const AudioResourceInfo &GetAudioInfo() const;

        void CleanUp();
    };
}