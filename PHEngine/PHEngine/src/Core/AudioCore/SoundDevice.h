#pragma once

#include <AL/alc.h>
#include <memory>

namespace EngineCore
{
    class SoundDevice
    {
        static std::shared_ptr<SoundDevice> s_mePtr;

        ALCdevice *m_alcDevice;
        ALCcontext *m_alcContext;

        bool mIsCleanedUp;

    public:
        SoundDevice();

        ~SoundDevice();

        void CleanUp();

        static std::shared_ptr<SoundDevice> GetInstance();
    };
}
