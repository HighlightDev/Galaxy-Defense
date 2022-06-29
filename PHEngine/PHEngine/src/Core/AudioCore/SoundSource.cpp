#include "SoundSource.h"
#include "Core/AudioCore/SoundBuffer.h"

#include <iostream>
#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace EngineCore
{
    SoundSource::SoundSource()
        : mPitch(1.f),
          mGain(1.f),
          mPosition(0.0f),
          mVelocity(0.0f),
          mIsLoopSound(false),
          mActiveBuffer()
    {
        Logger::Out("SoundSource::ctor");
        Init();
    }

    SoundSource::~SoundSource()
    {
        Logger::Out("SoundSource::dctor => mSourceDesc = ", mSourceDesc);
        alDeleteSources(1, &mSourceDesc);
    }

    void SoundSource::SetPitch(const float pitch)
    {
        mPitch = pitch;
        alSourcef(mSourceDesc, AL_PITCH, mPitch);
    }

    void SoundSource::SetGain(const float gain)
    {
        mGain = gain;
        alSourcef(mSourceDesc, AL_GAIN, mGain);
    }

    void SoundSource::SetPosition(const glm::vec3 &position)
    {
        mPosition = position;
        alSource3f(mSourceDesc, AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
    }

    void SoundSource::SetVelocity(const glm::vec3 &velocity)
    {
        mVelocity = velocity;
        alSource3f(mSourceDesc, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
    }

    void SoundSource::SetIsLoopSound(const bool isLoopSound)
    {
        if (mIsLoopSound != isLoopSound)
        {
            mIsLoopSound = isLoopSound;
            alSourcei(mSourceDesc, AL_LOOPING, mIsLoopSound);
        }
    }

    ALint SoundSource::GetCurrentSourceState() const
    {
        ALint state;
        alGetSourcei(mSourceDesc, AL_SOURCE_STATE, &state);
        return state;
    }

    float SoundSource::GetPitch() const
    {
        return mPitch;
    }

    float SoundSource::GetGain() const
    {
        return mGain;
    }

    glm::vec3 SoundSource::GetPosition() const
    {
        return mPosition;
    }

    glm::vec3 SoundSource::GetVelocity() const
    {
        return mVelocity;
    }

    bool SoundSource::GetIsLoopSound() const
    {
        return mIsLoopSound;
    }

    std::shared_ptr<SoundBuffer> SoundSource::GetActiveBuffer() const
    {
        return mActiveBuffer;
    }

    void SoundSource::Play(const std::shared_ptr<SoundBuffer> &soundBuffer)
    {
        if (!mActiveBuffer || soundBuffer->GetBufferDesc() != mActiveBuffer->GetBufferDesc())
        {
            mActiveBuffer = soundBuffer;
            alSourcei(mSourceDesc, AL_BUFFER, (ALint)mActiveBuffer->GetBufferDesc());
        }

        alSourcePlay(mSourceDesc);
    }

    void SoundSource::Init()
    {
        alGenSources(1, &mSourceDesc);
        alSourcef(mSourceDesc, AL_PITCH, mPitch);
        alSourcef(mSourceDesc, AL_GAIN, mGain);
        alSource3f(mSourceDesc, AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
        alSource3f(mSourceDesc, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
        alSourcei(mSourceDesc, AL_LOOPING, mIsLoopSound);

        Logger::Out("SoundSource::Init => mSourceDesc = ", mSourceDesc);
    }
}