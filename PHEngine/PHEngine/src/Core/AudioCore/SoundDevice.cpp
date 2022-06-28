#include "SoundDevice.h"

#include <AL/al.h>
#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace EngineCore
{
    SoundDevice *SoundDevice::s_mePtr = nullptr;

    SoundDevice::SoundDevice()
        : m_alcDevice(nullptr),
          m_alcContext(nullptr)
    {
        m_alcDevice = alcOpenDevice(nullptr); // get default device
        if (!m_alcDevice)
            throw("failed to get sound device");

        m_alcContext = alcCreateContext(m_alcDevice, nullptr);
        if (!m_alcContext)
            throw("failed to create context");

        if (!alcMakeContextCurrent(m_alcContext))
            throw("failed to make context current");

        const ALCchar *name = nullptr;
        if (alcIsExtensionPresent(m_alcDevice, "ALC_ENUMERATE_ALL_EXT"))
            name = alcGetString(m_alcDevice, ALC_ALL_DEVICES_SPECIFIER);
        if (!name || alcGetError(m_alcDevice) != AL_NO_ERROR)
            name = alcGetString(m_alcDevice, ALC_DEVICE_SPECIFIER);

        Logger::Out("SoundDevice::ctor => Opened device ", name);
    }

    SoundDevice::~SoundDevice()
    {
        if (!alcMakeContextCurrent(nullptr))
        {
            Logger::Out("SoundDevice::~SoundDevice => failed to set context to nullptr");
        }

        alcDestroyContext(m_alcContext);
        if (m_alcContext)
        {
            Logger::Out("SoundDevice::~SoundDevice => failed to unset during close");
        }

        if (!alcCloseDevice(m_alcDevice))
        {
            Logger::Out("SoundDevice::~SoundDevice => failed to close sound device");
        }
    }

    SoundDevice *SoundDevice::GetInstance()
    {
        if (!s_mePtr)
        {
            s_mePtr = new SoundDevice();
        }

        return s_mePtr;
    }
}
