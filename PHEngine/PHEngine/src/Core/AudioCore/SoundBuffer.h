#pragma once

#include <AL/al.h>
#include <AL/alext.h>

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"

using namespace IO::Audio;

namespace EngineCore
{
    class SoundBuffer
    {
        ALuint mBufferDesc;

    public:
        SoundBuffer(const ALvoid *soundData, const AudioResourceInfo &audioInfo);

        ~SoundBuffer();

        void CleanUp();


        ALuint GetBufferDesc() const;

    private:
        void Init(const ALvoid *soundData, const AudioResourceInfo &audioInfo);
    };
}