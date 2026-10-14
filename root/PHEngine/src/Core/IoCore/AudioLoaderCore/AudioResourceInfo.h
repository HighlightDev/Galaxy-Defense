#pragma once

#include <AL/alext.h>
#include <stdint.h>

#include <cstdint>

namespace IO {
namespace Audio {
struct AudioResourceInfo {
    ALenum mAudioFormat;
    int32_t mSampleRate;
    size_t mChannelsCount;
    ALsizei mNumBytes;
};
} // namespace Audio
} // namespace IO