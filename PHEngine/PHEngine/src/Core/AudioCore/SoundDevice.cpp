#include "SoundDevice.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <AL/al.h>

namespace EngineCore {
std::shared_ptr<SoundDevice> SoundDevice::s_mePtr = nullptr;

SoundDevice::SoundDevice()
    : m_alcDevice(nullptr)
    , m_alcContext(nullptr)
    , mIsCleanedUp(false)
{
    m_alcDevice = alcOpenDevice(nullptr); // get default device
    if (!m_alcDevice)
        throw("failed to get sound device");

    m_alcContext = alcCreateContext(m_alcDevice, nullptr);
    if (!m_alcContext)
        throw("failed to create context");

    if (!alcMakeContextCurrent(m_alcContext))
        throw("failed to make context current");

    const ALCchar* name = nullptr;
    if (alcIsExtensionPresent(m_alcDevice, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(m_alcDevice, ALC_ALL_DEVICES_SPECIFIER);
    if (!name || alcGetError(m_alcDevice) != AL_NO_ERROR)
        name = alcGetString(m_alcDevice, ALC_DEVICE_SPECIFIER);

    LogInfo("SoundDevice::ctor => Opened device ", name);
}

void SoundDevice::CleanUp()
{
    if (!alcMakeContextCurrent(nullptr)) {
        LogInfo("SoundDevice::CleanUp => failed to set context to nullptr");
    }

    alcDestroyContext(m_alcContext);
    if (m_alcContext) {
        LogInfo("SoundDevice::CleanUp => failed to unset during close");
    }

    if (!alcCloseDevice(m_alcDevice)) {
        LogInfo("SoundDevice::CleanUp => failed to close sound device");
    }

    mIsCleanedUp = true;
}

SoundDevice::~SoundDevice()
{
    assert(mIsCleanedUp); // clean up should be called before dctor
}

std::shared_ptr<SoundDevice> SoundDevice::GetInstance()
{
    if (!s_mePtr) {
        s_mePtr = std::make_shared<SoundDevice>();
    }

    return s_mePtr;
}
} // namespace EngineCore
