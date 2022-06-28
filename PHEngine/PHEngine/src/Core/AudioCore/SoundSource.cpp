#include "SoundSource.h"
#include "Core/AudioCore/SoundBuffer.h"

#include <iostream>

namespace EngineCore
{
    SoundSource::SoundSource()
        : mActiveBuffer()
    {
        alGenSources(1, &mSourceDesc);
        alSourcef(mSourceDesc, AL_PITCH, p_Pitch);
        alSourcef(mSourceDesc, AL_GAIN, p_Gain);
        alSource3f(mSourceDesc, AL_POSITION, p_Position[0], p_Position[1], p_Position[2]);
        alSource3f(mSourceDesc, AL_VELOCITY, p_Velocity[0], p_Velocity[1], p_Velocity[2]);
        alSourcei(mSourceDesc, AL_LOOPING, p_LoopSound);
    }

    SoundSource::~SoundSource()
    {
        alDeleteSources(1, &mSourceDesc);
    }

    void SoundSource::Play(const std::shared_ptr<SoundBuffer>& soundBuffer)
    {
        if (!mActiveBuffer || soundBuffer->GetBufferDesc() != mActiveBuffer->GetBufferDesc())
        {
            mActiveBuffer = soundBuffer;
            alSourcei(mSourceDesc, AL_BUFFER, (ALint)mActiveBuffer->GetBufferDesc());
        }

        alSourcePlay(mSourceDesc);

        ALint state = AL_PLAYING;
        std::cout << "playing sound\n";
        if (state == AL_PLAYING && alGetError() == AL_NO_ERROR)
        {
            std::cout << "currently playing sound\n";
            //alGetSourcei(mSourceDesc, AL_SOURCE_STATE, &state);
        }
        std::cout << "done playing sound\n";
    }
}