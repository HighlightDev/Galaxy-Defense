#include "SoundBuffer.h"
#include "Core/CommonCore/Assertion.h"

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace EngineCore
{
    SoundBuffer::SoundBuffer(const ALvoid *soundData, const AudioResourceInfo &audioInfo)
        : mBufferDesc(0)
    {
        Logger::Out("SoundBuffer::ctor");
        Init(soundData, audioInfo);
    }

    SoundBuffer::~SoundBuffer()
    {
        Logger::Out("SoundBuffer::dctor");
    }

    void SoundBuffer::CleanUp()
    {
        Logger::Out("SoundBuffer::CleanUp => BufferDesc = ", mBufferDesc);
        alDeleteBuffers(1, &mBufferDesc);
    }

    void SoundBuffer::Init(const ALvoid *soundData, const AudioResourceInfo &audioInfo)
    {
        alGenBuffers(1, &mBufferDesc);
        alBufferData(mBufferDesc, audioInfo.mAudioFormat, soundData, audioInfo.mNumBytes, audioInfo.mSampleRate);

        const auto err = alGetError();
        Logger::Out("SoundBuffer::Init => OpenAL error : ", err);
        assert(err == AL_NO_ERROR);
    }

    ALuint SoundBuffer::GetBufferDesc() const
    {
        return mBufferDesc;
    }
}