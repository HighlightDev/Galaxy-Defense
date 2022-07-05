#include "SoundSource.h"
#include "Core/AudioCore/SoundBuffer.h"
#include "Core/AudioCore/ErrorHandler.h"
#include "Core/GameCore/LoggerExtension.h"

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
        LogInfo( "SoundSource::ctor");
        Init();
    }

    SoundSource::~SoundSource()
    {
        LogInfo( "SoundSource::dctor => mSourceDesc = ", mSourceDesc);
        alDeleteSources(1, &mSourceDesc);
    }

    void SoundSource::SetPitch(const float pitch)
    {
        mPitch = pitch;
        alCall(alSourcef, mSourceDesc, AL_PITCH, mPitch);
    }

    void SoundSource::SetGain(const float gain)
    {
        mGain = gain;
        alCall(alSourcef, mSourceDesc, AL_GAIN, mGain);
    }

    void SoundSource::SetPosition(const glm::vec3 &position)
    {
        mPosition = position;
        alCall(alSource3f, mSourceDesc, AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
    }

    void SoundSource::SetVelocity(const glm::vec3 &velocity)
    {
        mVelocity = velocity;
        alCall(alSource3f, mSourceDesc, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
    }

    void SoundSource::SetIsLoopSound(const bool isLoopSound)
    {
        if (mIsLoopSound != isLoopSound)
        {
            mIsLoopSound = isLoopSound;
            alCall(alSourcei, mSourceDesc, AL_LOOPING, mIsLoopSound);
        }
    }

    ALint SoundSource::GetCurrentSourceState() const
    {
        ALint state;
        alCall(alGetSourcei, mSourceDesc, AL_SOURCE_STATE, &state);
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

    bool SoundSource::IsSoundLooped() const
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
            alCall(alSourcei, mSourceDesc, AL_BUFFER, (ALint)mActiveBuffer->GetBufferDesc());
        }

        alCall(alSourcePlay, mSourceDesc);
    }

    void SoundSource::Stop()
    {
        if (!mActiveBuffer || GetCurrentSourceState() != AL_PLAYING)
            return;
        
        alCall(alSourceStop, mSourceDesc);
    }

    void SoundSource::Init()
    {
        alCall(alGenSources, 1, &mSourceDesc);
        alCall(alSourcef, mSourceDesc, AL_PITCH, mPitch);
        alCall(alSourcef, mSourceDesc, AL_GAIN, mGain);
        alCall(alSource3f, mSourceDesc, AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
        alCall(alSource3f, mSourceDesc, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
        alCall(alSourcei, mSourceDesc, AL_LOOPING, mIsLoopSound);

        LogInfo( "SoundSource::Init => mSourceDesc = ", mSourceDesc);
    }
}