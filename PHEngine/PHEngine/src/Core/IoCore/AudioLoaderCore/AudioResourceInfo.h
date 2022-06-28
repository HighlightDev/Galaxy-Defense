#pragma once

#include <stdint.h>
#include <cstdint>

#include <AL/alext.h>

namespace IO
{
    namespace Audio
    {
        struct AudioResourceInfo
        {
			ALenum mAudioFormat;
            int32_t mSampleRate;
			ALsizei mNumBytes;
        };
    }
}