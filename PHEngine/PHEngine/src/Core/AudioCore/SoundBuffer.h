#pragma once

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

#include <AL/al.h>
#include <AL/alext.h>

#include <string>

using namespace IO::Audio;

namespace EngineCore {
class SoundBuffer {
    ALuint mBufferDesc;

    std::string mSoundName;

public:
    SoundBuffer(const ALvoid* soundData, const AudioResourceInfo& audioInfo, const std::string& soundName);

    ~SoundBuffer();

    bool operator==(const SoundBuffer& right) const;

    void CleanUp();

    ALuint GetBufferDesc() const;

    std::string GetSoundName() const;

private:
    void Init(const ALvoid* soundData, const AudioResourceInfo& audioInfo);
};
} // namespace EngineCore