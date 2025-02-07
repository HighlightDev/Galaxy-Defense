#pragma once
#include "Core/AudioCore/SoundStream.h"

#include <AL/al.h>

#include <memory>
#include <string>

namespace EngineCore {

class StreamingSoundBufferBundle {
    static constexpr size_t BUFFER_SAMPLES = 8192;

    static constexpr size_t NUM_BUFFERS = 4;

    ALuint mBuffers[NUM_BUFFERS];

    ALuint mSoundDescriptor;

    std::shared_ptr<SoundStream> mSoundMemoryChunk;

    size_t mBufferDataCursor = 0;

    bool mHasQueuedBuffers = false;

public:
    StreamingSoundBufferBundle(const std::string& soundName, const ALuint soundDescriptor);

    ~StreamingSoundBufferBundle();

    void CleanUp();

    void PrePlayFillBuffers(const ALuint sourceDesc);

    void UpdateBufferStream(const ALuint sourceDesc);

    bool IsStreamingFinished() const;

private:
    void Init(const std::string& soundName);
};
} // namespace EngineCore