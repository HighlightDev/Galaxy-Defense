#include "SndFileLoader.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <AL/alext.h>
#include <sndfile/sndfile.h>

#include <limits>

namespace IO {
namespace Audio {
SndFileLoader::SndFileLoader()
    : m_lastAllocatedMemory(nullptr)
{
}

SndFileLoader::~SndFileLoader()
{
    ReleaseAudioMemory();
}

void* SndFileLoader::AllocateMemoryForAudioSource(const std::string& pathToFile, AudioResourceInfo& outAudioInfo)
{
    ReleaseAudioMemory(); // Release previously allocated memory

    SNDFILE* sndfile;
    SF_INFO sfinfo;
    short* membuf;
    sf_count_t num_frames;

    // Open the audio file and check that it's usable.
    sndfile = sf_open(pathToFile.c_str(), SFM_READ, &sfinfo);
    ext_assert(sndfile, "SndFileLoader::AllocateMemoryForAudioSource: Failed to open audio file: " + pathToFile);

    const bool isBadSample
        = (sfinfo.frames < 1
           || sfinfo.frames > (sf_count_t)(std::numeric_limits<int64_t>::max() / sizeof(short)) / sfinfo.channels);
    ext_assert(!isBadSample, "SndFileLoader::AllocateMemoryForAudioSource: Bad sample in audio file: " + pathToFile);

    // Get the sound format, and figure out the OpenAL format

    outAudioInfo.mAudioFormat = AL_NONE;
    switch (sfinfo.channels) {
    case 1:
        outAudioInfo.mAudioFormat = AL_FORMAT_MONO16;
        break;
    case 2:
        outAudioInfo.mAudioFormat = AL_FORMAT_STEREO16;
        break;
    case 3: {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            outAudioInfo.mAudioFormat = AL_FORMAT_BFORMAT2D_16;
        break;
    }
    case 4: {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            outAudioInfo.mAudioFormat = AL_FORMAT_BFORMAT3D_16;
        break;
    }
    default:
        ext_assert(
            false, "SndFileLoader::AllocateMemoryForAudioSource: Unsupported channel count: " + std::to_string(sfinfo.channels));
    }

    outAudioInfo.mSampleRate = sfinfo.samplerate;
    outAudioInfo.mChannelsCount = (size_t)sfinfo.channels;

    // Decode the whole audio file to a buffer.
    membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

    num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
    ext_assert(
        num_frames >= 1, "SndFileLoader::AllocateMemoryForAudioSource: Failed to read samples from audio file: " + pathToFile);
    outAudioInfo.mNumBytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

    sf_close(sndfile);

    m_lastAllocatedMemory = (void*)membuf;

    return m_lastAllocatedMemory;
}

void SndFileLoader::ReleaseAudioMemory()
{
    if (m_lastAllocatedMemory) {
        free(m_lastAllocatedMemory);
        m_lastAllocatedMemory = nullptr;
    }
}
} // namespace Audio
} // namespace IO
