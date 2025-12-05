#include "StbImlementation.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <AL/alext.h>
#include <stb/stb_vorbis.h>

#include <limits>

namespace IO {
namespace Audio {
StbFileLoader::StbFileLoader()
    : m_lastAllocatedMemory(nullptr)
{
}

StbFileLoader::~StbFileLoader()
{
    ReleaseAudioMemory();
}

void* StbFileLoader::AllocateMemoryForAudioSource(const std::string& pathToFile, AudioResourceInfo& outAudioInfo)
{
    ReleaseAudioMemory(); // Release previously allocated memory

    int32_t channels, sample_rate, samples;
    short* membuf;

    samples = stb_vorbis_decode_filename(pathToFile.c_str(), &channels, &sample_rate, &membuf);

    // Get the sound format, and figure out the OpenAL format
    outAudioInfo.mAudioFormat = AL_NONE;
    switch (channels) {
    case 1:
        outAudioInfo.mAudioFormat = AL_FORMAT_MONO16;
        break;
    case 2:
        outAudioInfo.mAudioFormat = AL_FORMAT_STEREO16;
        break;
    default:
        ext_assert(false, "Unsupported channel count: " + std::to_string(channels));
    }

    ext_assert(samples >= 1, "Failed to read samples from file: " + pathToFile);
    outAudioInfo.mSampleRate = sample_rate;
    outAudioInfo.mChannelsCount = channels;
    outAudioInfo.mNumBytes = (ALsizei)(samples * channels) * (ALsizei)sizeof(short);

    m_lastAllocatedMemory = (void*)membuf;

    return m_lastAllocatedMemory;
}

std::shared_ptr<StbSoundStream> StbFileLoader::OpenStream(const std::string& pathToFile, AudioResourceInfo& outAudioInfo)
{
    stb_vorbis* stream = stb_vorbis_open_filename(pathToFile.c_str(), nullptr, nullptr);
    const stb_vorbis_info info = stb_vorbis_get_info(stream);

    const auto samples = stb_vorbis_stream_length_in_samples(stream) * info.channels;

    outAudioInfo.mAudioFormat = AL_NONE;
    switch (info.channels) {
    case 1:
        outAudioInfo.mAudioFormat = AL_FORMAT_MONO16;
        break;
    case 2:
        outAudioInfo.mAudioFormat = AL_FORMAT_STEREO16;
        break;
    default:
        ext_assert(false, "Unsupported channel count: " + std::to_string(info.channels)); // Unsupported channel count
    }

    ext_assert(samples >= 1, "Failed to read samples from file: " + pathToFile); // Failed to read samples
    outAudioInfo.mSampleRate = info.sample_rate;
    outAudioInfo.mChannelsCount = info.channels;
    outAudioInfo.mNumBytes = (ALsizei)(samples * info.channels) * (ALsizei)sizeof(short);

    return std::make_shared<StbSoundStream>(stream, outAudioInfo);
}

void StbFileLoader::ReleaseAudioMemory()
{
    if (m_lastAllocatedMemory) {
        free(m_lastAllocatedMemory);
        m_lastAllocatedMemory = nullptr;
    }
}

/*
 * StbSoundStream
 */

static constexpr uint32_t CHUNK_SIZE = 65536;

StbSoundStream::StbSoundStream(::stb_vorbis* stream, const AudioResourceInfo& audioInfo)
    : mStream(stream)
    , mAudioInfo(audioInfo)
{
}

StbSoundStream::~StbSoundStream()
{
    CleanUp();
}

short* StbSoundStream::GetDataChunk() const
{
    return mDataChunk;
}

int32_t StbSoundStream::ReadNewDataPortionIntoChunk()
{
    if (!mDataChunk) {
        mDataChunk = static_cast<short*>(malloc(CHUNK_SIZE * sizeof(short)));
    }

    mLastReadSamplesCount = stb_vorbis_get_samples_short_interleaved(mStream, mAudioInfo.mChannelsCount, mDataChunk, CHUNK_SIZE);
    return mLastReadSamplesCount;
}

int32_t StbSoundStream::GetLastReadDataSamplesCount() const
{
    return mLastReadSamplesCount;
}

void StbSoundStream::ReadStreamFromStart()
{
    stb_vorbis_seek_start(mStream);
}

void StbSoundStream::CleanUp()
{
    if (mStream) {
        stb_vorbis_close(mStream);
        mStream = nullptr;
    }

    if (mDataChunk) {
        free(mDataChunk);
        mDataChunk = nullptr;
    }
}
} // namespace Audio
} // namespace IO
