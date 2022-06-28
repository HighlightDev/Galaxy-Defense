#pragma once

#include <AL/al.h>
#include <AL/alext.h>

namespace EngineCore
{
    class SoundBuffer
    {
        public:

        SoundBuffer();

        ~SoundBuffer();

        void CleanUp();
    };
}