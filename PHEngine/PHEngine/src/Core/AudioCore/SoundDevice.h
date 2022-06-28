#pragma once

#include <AL/alc.h>

namespace EngineCore
{
    class SoundDevice
    {
        static SoundDevice* s_mePtr;

        ALCdevice* m_alcDevice;
        ALCcontext* m_alcContext;

        SoundDevice();

        ~SoundDevice();

        public:

        static SoundDevice* GetInstance();
    };
}
