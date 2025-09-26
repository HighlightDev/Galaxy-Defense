#pragma once

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

#include <memory>

using namespace IO::Audio;

namespace IO::Audio {
class StbSoundStream;
}

namespace EngineCore {
class SoundStream {

    std::string mStreamName;

    std::shared_ptr<StbSoundStream> mStream;

    AudioResourceInfo mAudioInfo;

public:
    SoundStream(const std::shared_ptr<StbSoundStream>& stream, const AudioResourceInfo& audioInfo, const std::string& streamName);

    bool operator==(const SoundStream& right) const;

    short* GetCurrentDataChunk() const;

    int32_t ReadNewDataPortionIntoChunk();

    int32_t GetLastReadDataSamplesCount() const;

    void ReadStreamFromStart() const;

    const AudioResourceInfo& GetAudioInfo() const;

    std::string GetStreamName() const;

    void CleanUp();
};
} // namespace EngineCore