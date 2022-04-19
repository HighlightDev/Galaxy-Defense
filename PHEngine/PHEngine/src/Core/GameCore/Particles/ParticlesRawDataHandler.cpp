#include "ParticlesRawDataHandler.h"

#include "Core/CommonCore/Assertion.h"

#include <cstdlib>
#include <cstring>

namespace EngineCore
{

    ParticlesRawDataHandler::ParticlesRawDataHandler(const size_t particlesCount)
        : mTranslationData(nullptr),
          mTranslationDataSize(GetTranslationVectorByteDataOffset() * particlesCount),
          mTranslationActiveDataChunkSize(GetTranslationVectorByteDataOffset() * particlesCount),
          mRotationSizeData(nullptr),
          mRotationSizeDataSize(GetRotationSizeByteDataOffset() * particlesCount),
          mRotationSizeActiveDataChunkSize(GetRotationSizeByteDataOffset() * particlesCount)
    {
        assert(particlesCount);
        AllocatePoolMemory();
    }

    ParticlesRawDataHandler::~ParticlesRawDataHandler()
    {
        DeallocatePoolMemory();
    }

    void ParticlesRawDataHandler::SubTranslationData(const size_t byteDataOffset, const glm::vec3 &translation)
    {
        assert(mTranslationData);
        assert(byteDataOffset <= (mTranslationDataSize - GetTranslationVectorByteDataOffset()));

        float *f_data = (float *)((char *)mTranslationData + byteDataOffset);
        f_data[0] = translation.x;
        f_data[1] = translation.y;
        f_data[2] = translation.z;
    }

    void *ParticlesRawDataHandler::CopyToDstActiveTranslationData(void *dst)
    {
        return memcpy(dst, mTranslationData, mTranslationActiveDataChunkSize);
    }

    void ParticlesRawDataHandler::CopyToMeActiveTranslationData(const void *src, const size_t offset, const size_t byteChunkSize)
    {
        assert(mTranslationDataSize >= (byteChunkSize - offset));
        (void)memcpy((void *)((char *)mTranslationData + offset), src, byteChunkSize);
    }

    void ParticlesRawDataHandler::SetTranslationActiveDataChunkSize(const size_t activeDataChunkSize)
    {
        assert(activeDataChunkSize <= mTranslationDataSize);
        mTranslationActiveDataChunkSize = activeDataChunkSize;
    }

    size_t ParticlesRawDataHandler::GetTranslationDataSize() const
    {
        return mTranslationDataSize;
    }

    size_t ParticlesRawDataHandler::GetTranslationActiveDataChunkSize() const
    {
        return mTranslationActiveDataChunkSize;
    }

    void *ParticlesRawDataHandler::GetTranslationData() const
    {
        return mTranslationData;
    }

    void ParticlesRawDataHandler::SubRotationSizeData(const size_t byteDataOffset, const float rotation, const float size)
    {
        assert(mRotationSizeData);
        assert(byteDataOffset <= (mRotationSizeDataSize - GetRotationSizeByteDataOffset()));

        float *f_data = (float *)((char *)mRotationSizeData + byteDataOffset);
        f_data[0] = rotation;
        f_data[1] = size;
    }

    void ParticlesRawDataHandler::SetRotationSizeActiveDataChunkSize(const size_t activeDataChunkSize)
    {
        assert(activeDataChunkSize <= mRotationSizeDataSize);
        mRotationSizeActiveDataChunkSize = activeDataChunkSize;
    }

    size_t ParticlesRawDataHandler::GetRotationSizeDataSize() const
    {
        return mRotationSizeDataSize;
    }

    size_t ParticlesRawDataHandler::GetRotationSizeActiveDataChunkSize() const
    {
        return mRotationSizeActiveDataChunkSize;
    }

    void *ParticlesRawDataHandler::GetRotationSizeData() const
    {
        return mRotationSizeData;
    }

    void *ParticlesRawDataHandler::CopyToDstActiveRotationSizeData(void *dst)
    {
        return memcpy(dst, mRotationSizeData, mRotationSizeActiveDataChunkSize);
    }

    void ParticlesRawDataHandler::CopyToMeActiveRotationSizeData(const void *src, const size_t offset, const size_t byteChunkSize)
    {
        assert(mRotationSizeDataSize >= (byteChunkSize - offset));
        (void)memcpy((void *)((char *)mRotationSizeData + offset), src, byteChunkSize);
    }

    void ParticlesRawDataHandler::AllocatePoolMemory()
    {
        mTranslationData = malloc(mTranslationDataSize);
        mRotationSizeData = malloc(mRotationSizeDataSize);
    }

    void ParticlesRawDataHandler::DeallocatePoolMemory()
    {
        if (mTranslationData)
            free(mTranslationData);

        if (mRotationSizeData)
            free(mRotationSizeData);
    }
}