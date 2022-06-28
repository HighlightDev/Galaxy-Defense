#pragma once

#include <memory>
#include <AL/al.h>

namespace EngineCore
{
    class SoundBuffer;

    class SoundSource
    {
    public:
        SoundSource();
        ~SoundSource();

        void Play(const std::shared_ptr<SoundBuffer>& soundBuffer);

    private:
        ALuint mSourceDesc;
        float p_Pitch = 1.f;
        float p_Gain = 1.f;
        float p_Position[3] = {0, 0, 0};
        float p_Velocity[3] = {0, 0, 0};
        bool p_LoopSound = false;
        
        std::shared_ptr<SoundBuffer> mActiveBuffer;
    };

}
