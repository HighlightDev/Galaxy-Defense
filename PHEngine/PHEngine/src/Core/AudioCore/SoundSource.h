#pragma once

#include <AL/al.h>
#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore {
class SoundBuffer;

class SoundSource {
    ALuint mSourceDesc;

    float mPitch;

    float mGain;

    glm::vec3 mPosition;

    glm::vec3 mVelocity;

    bool mIsLoopSound;

    std::shared_ptr<SoundBuffer> mActiveBuffer;

public:
    SoundSource();

    ~SoundSource();

    void SetPitch(const float pitch);

    void SetGain(const float gain);

    void SetPosition(const glm::vec3& position);

    void SetVelocity(const glm::vec3& velocity);

    void SetIsLoopSound(const bool isLoopSound);

    void Play(const std::shared_ptr<SoundBuffer>& soundBuffer);

    void Stop();

    ALint GetCurrentSourceState() const;

    float GetPitch() const;

    float GetGain() const;

    glm::vec3 GetPosition() const;

    glm::vec3 GetVelocity() const;

    bool IsSoundLooped() const;

    std::shared_ptr<SoundBuffer> GetActiveBuffer() const;

private:
    void Init();
};

} // namespace EngineCore
