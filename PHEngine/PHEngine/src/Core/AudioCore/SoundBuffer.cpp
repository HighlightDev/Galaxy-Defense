#include "SoundBuffer.h"

#include "Core/AudioCore/ErrorHandler.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

namespace EngineCore {
SoundBuffer::SoundBuffer(const ALvoid* soundData, const AudioResourceInfo& audioInfo, const std::string& soundName)
    : mBufferDesc(0)
    , mSoundName(soundName)
{
    LogInfo("SoundBuffer::ctor");
    Init(soundData, audioInfo);
}

SoundBuffer::~SoundBuffer()
{
    LogInfo("SoundBuffer::dctor");
}

bool SoundBuffer::operator==(const SoundBuffer& right) const
{
    return this->mBufferDesc == right.mBufferDesc;
}

void SoundBuffer::CleanUp()
{
    LogInfo("SoundBuffer::CleanUp: BufferDesc = ", mBufferDesc);
    alDeleteBuffers, 1, &mBufferDesc;
}

void SoundBuffer::Init(const ALvoid* soundData, const AudioResourceInfo& audioInfo)
{
    alCall(alGenBuffers, 1, &mBufferDesc);
    alCall(alBufferData, mBufferDesc, audioInfo.mAudioFormat, soundData, audioInfo.mNumBytes, audioInfo.mSampleRate);
}

ALuint SoundBuffer::GetBufferDesc() const
{
    return mBufferDesc;
}

std::string SoundBuffer::GetSoundName() const
{
    return mSoundName;
}
} // namespace EngineCore