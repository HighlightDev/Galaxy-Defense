#pragma once

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

#include <AL/al.h>
#include <AL/alext.h>

using namespace IO::Audio;

namespace EngineCore {
class SoundBuffer {
    ALuint mBufferDesc;

public:
    SoundBuffer(const ALvoid* soundData, const AudioResourceInfo& audioInfo);

    ~SoundBuffer();

    bool operator==(const SoundBuffer& right) const;

    void CleanUp();

    ALuint GetBufferDesc() const;

private:
    void Init(const ALvoid* soundData, const AudioResourceInfo& audioInfo);
};
} // namespace EngineCore