#include "SoundBuffer.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/AudioCore/ErrorHandler.h"

namespace EngineCore
{
    SoundBuffer::SoundBuffer(const ALvoid *soundData, const AudioResourceInfo &audioInfo)
        : mBufferDesc(0)
    {
        LogInfo("SoundBuffer::ctor");
        Init(soundData, audioInfo);
    }

    SoundBuffer::~SoundBuffer()
    {
        LogInfo("SoundBuffer::dctor");
    }

    void SoundBuffer::CleanUp()
    {
        LogInfo("SoundBuffer::CleanUp => BufferDesc = ", mBufferDesc);
        alDeleteBuffers, 1, &mBufferDesc;
    }

    void SoundBuffer::Init(const ALvoid *soundData, const AudioResourceInfo &audioInfo)
    {
        alCall(alGenBuffers, 1, &mBufferDesc);
        alCall(alBufferData, mBufferDesc, audioInfo.mAudioFormat, soundData, audioInfo.mNumBytes, audioInfo.mSampleRate);
    }

    ALuint SoundBuffer::GetBufferDesc() const
    {
        return mBufferDesc;
    }
}