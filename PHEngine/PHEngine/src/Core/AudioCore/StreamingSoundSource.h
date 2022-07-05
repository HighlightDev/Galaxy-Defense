#pragma once

#include <memory>
#include <AL/al.h>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class StreamingSoundBufferBundle;

    class StreamingSoundSource
    {
        ALuint mSourceDesc;

        float mPitch;

        float mGain;

        glm::vec3 mPosition;

        glm::vec3 mVelocity;

        bool mIsPlaybackEnabled;

        bool mIsLoopSound;

        std::shared_ptr<StreamingSoundBufferBundle> mStreamingBufferBundle;

    public:
        StreamingSoundSource(const std::string& soundName);

        ~StreamingSoundSource();

        void SetPitch(const float pitch);

        void SetGain(const float gain);

        void SetPosition(const glm::vec3& position);

        void SetVelocity(const glm::vec3& velocity);

        void SetIsLoopSound(const bool isLoopSound);

        void Play();

        void Stop();

        ALint GetCurrentSourceState() const;

        float GetPitch() const;

        float GetGain() const;
        
        glm::vec3 GetPosition() const;

        glm::vec3 GetVelocity() const;

        bool IsSoundLooped() const;

        void UpdateBufferStream();

        bool IsPlaying() const;

    private:
        void Init(const std::string& soundName);
    };

}
