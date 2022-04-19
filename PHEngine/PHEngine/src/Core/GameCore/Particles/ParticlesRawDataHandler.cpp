#include "ParticlesRawDataHandler.h"

#include "Core/CommonCore/Assertion.h"

#include <cstdlib>
#include <cstring>

namespace EngineCore
{

    ParticlesRawDataHandler::ParticlesRawDataHandler(const size_t dataSize)
        : mTranslationData(nullptr),
          mTranslationDataSize(dataSize),
          mActiveDataChunkSize(dataSize)
    {
        assert(dataSize);
        AllocatePoolMemory(mTranslationDataSize);
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
        return memcpy(dst, mTranslationData, mActiveDataChunkSize);
    }

    void ParticlesRawDataHandler::CopyToMeActiveTranslationData(const void *src, const size_t offset, const size_t byteChunkSize)
    {
        assert(mTranslationDataSize >= (byteChunkSize - offset));
        (void)memcpy((void *)((char *)mTranslationData + offset), src, byteChunkSize);
    }

    void ParticlesRawDataHandler::SetActiveDataChunkSize(const size_t activeDataChunkSize)
    {
        assert(activeDataChunkSize <= mTranslationDataSize);
        mActiveDataChunkSize = activeDataChunkSize;
    }

    size_t ParticlesRawDataHandler::GetTranslationDataSize() const
    {
        return mTranslationDataSize;
    }

    size_t ParticlesRawDataHandler::GetActiveDataChunkSize() const
    {
        return mActiveDataChunkSize;
    }

    void *ParticlesRawDataHandler::GetTranslationData() const
    {
        return mTranslationData;
    }

    void ParticlesRawDataHandler::AllocatePoolMemory(const size_t dataSize)
    {
        mTranslationData = malloc(dataSize);
    }

    void ParticlesRawDataHandler::DeallocatePoolMemory()
    {
        if (mTranslationData)
            free(mTranslationData);
    }
}