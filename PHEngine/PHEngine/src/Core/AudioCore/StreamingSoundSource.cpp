#include "StreamingSoundSource.h"
#include "Core/AudioCore/StreamingSoundBufferBundle.h"
#include "Core/AudioCore/ErrorHandler.h"
#include "Core/GameCore/LoggerExtension.h"

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace EngineCore
{
    StreamingSoundSource::StreamingSoundSource(const std::string &soundName)
        : mPitch(1.f),
          mGain(1.f),
          mPosition(0.0f),
          mVelocity(0.0f),
          mIsPlaybackEnabled(false),
          mIsLoopSound(false),
          mStreamingBufferBundle()
    {
        LogInfo("StreamingSoundSource::ctor");
        Init(soundName);
    }

    StreamingSoundSource::~StreamingSoundSource()
    {
        LogInfo("StreamingSoundSource::dctor => mSourceDesc = ", mSourceDesc);
        alDeleteSources(1, &mSourceDesc);
    }

    void StreamingSoundSource::SetPitch(const float pitch)
    {
        mPitch = pitch;
        alCall(alSourcef, mSourceDesc, AL_PITCH, mPitch);
    }

    void StreamingSoundSource::SetGain(const float gain)
    {
        mGain = gain;
        alCall(alSourcef, mSourceDesc, AL_GAIN, mGain);
    }

    void StreamingSoundSource::SetPosition(const glm::vec3 &position)
    {
        mPosition = position;
        alCall(alSource3f, mSourceDesc, AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
    }

    void StreamingSoundSource::SetVelocity(const glm::vec3 &velocity)
    {
        mVelocity = velocity;
        alCall(alSource3f, mSourceDesc, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
    }

    void StreamingSoundSource::SetIsLoopSound(const bool isLoopSound)
    {
        mIsLoopSound = isLoopSound;
    }

    ALint StreamingSoundSource::GetCurrentSourceState() const
    {
        ALint state;
        alCall(alGetSourcei, mSourceDesc, AL_SOURCE_STATE, &state);
        return state;
    }

    float StreamingSoundSource::GetPitch() const
    {
        return mPitch;
    }

    float StreamingSoundSource::GetGain() const
    {
        return mGain;
    }

    glm::vec3 StreamingSoundSource::GetPosition() const
    {
        return mPosition;
    }

    glm::vec3 StreamingSoundSource::GetVelocity() const
    {
        return mVelocity;
    }

    bool StreamingSoundSource::IsSoundLooped() const
    {
        return mIsLoopSound;
    }

    bool StreamingSoundSource::IsPlaying() const
    {
        return (GetCurrentSourceState() == AL_PLAYING);
    }

    void StreamingSoundSource::Play()
    {
        mStreamingBufferBundle->PrePlayFillBuffers(mSourceDesc);
        alCall(alSourcePlay, mSourceDesc);
        mIsPlaybackEnabled = true;
    }

    void StreamingSoundSource::Stop()
    {
        if (GetCurrentSourceState() != AL_PLAYING)
            return;

        alCall(alSourceStop, mSourceDesc);
        mIsPlaybackEnabled = false;
    }

    void StreamingSoundSource::UpdateBufferStream()
    {
        if (!mIsPlaybackEnabled)
            return;

        mStreamingBufferBundle->UpdateBufferStream(mSourceDesc);

        if (mIsLoopSound && mStreamingBufferBundle->IsStreamingFinished())
        {
            Play();
        }
    }

    void StreamingSoundSource::Init(const std::string &soundName)
    {
        alCall(alGenSources, 1, &mSourceDesc);
        alCall(alSourcef, mSourceDesc, AL_PITCH, mPitch);
        alCall(alSourcef, mSourceDesc, AL_GAIN, mGain);
        alCall(alSource3f, mSourceDesc, AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
        alCall(alSource3f, mSourceDesc, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);

        mStreamingBufferBundle = std::make_shared<StreamingSoundBufferBundle>(soundName);

        LogInfo("StreamingSoundSource::Init => mSourceDesc = ", mSourceDesc);
    }
}