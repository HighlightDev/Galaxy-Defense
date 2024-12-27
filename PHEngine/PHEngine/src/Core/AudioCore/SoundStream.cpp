#include "SoundStream.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/IoCore/AudioLoaderCore/StbFileLoader/StbImlementation.h"

#include <stdlib.h>

namespace EngineCore
{
    SoundStream::SoundStream(const std::shared_ptr<StbSoundStream>& stream, const AudioResourceInfo &audioInfo)
        : mStream(stream),
          mAudioInfo(audioInfo)
    {
    }

    short *SoundStream::GetCurrentDataChunk() const
    {
        return mStream->GetDataChunk();
    }

    int32_t SoundStream::ReadNewDataPortionIntoChunk()
    {
        return mStream->ReadNewDataPortionIntoChunk();
    }

    int32_t SoundStream::GetLastReadDataSamplesCount() const
    {
        return mStream->GetLastReadDataSamplesCount();
    }

    void SoundStream::ReadStreamFromStart() const
    {
        mStream->ReadStreamFromStart();
    }

    bool SoundStream::operator==(const SoundStream &right) const
    {
        return this->mAudioInfo.mAudioFormat == right.mAudioInfo.mAudioFormat &&
               this->mAudioInfo.mChannelsCount == right.mAudioInfo.mChannelsCount &&
               this->mAudioInfo.mNumBytes == right.mAudioInfo.mNumBytes &&
               this->mAudioInfo.mSampleRate == right.mAudioInfo.mSampleRate;
    }

    const AudioResourceInfo &SoundStream::GetAudioInfo() const
    {
        return mAudioInfo;
    }

    void SoundStream::CleanUp()
    {
        mStream->CleanUp();
    }
}