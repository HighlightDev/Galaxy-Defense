#pragma once

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

#include <cstddef>
#include <string>

namespace IO {
namespace Audio {
class SndFileLoader {
    void* m_lastAllocatedMemory;

public:
    SndFileLoader();

    ~SndFileLoader();

    void* AllocateMemoryForAudioSource(const std::string& pathToFile, AudioResourceInfo& outAudioInfo);

    void ReleaseAudioMemory();
};
} // namespace Audio
} // namespace IO
