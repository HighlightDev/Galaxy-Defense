#include "ParticlesRawDataHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <cstdlib>
#include <cstring>

namespace EngineCore {

ParticlesRawDataHandler::ParticlesRawDataHandler(const size_t particlesCount)
    : mTranslationDataSize(GetTranslationVectorByteDataOffset() * particlesCount)
    , mTranslationActiveDataChunkSize(GetTranslationVectorByteDataOffset() * particlesCount)
    , mRotationSizeDataSize(GetRotationSizeByteDataOffset() * particlesCount)
    , mRotationSizeActiveDataChunkSize(GetRotationSizeByteDataOffset() * particlesCount)
    , mColorDataSize(GetColorByteDataOffset() * particlesCount)
    , mColorActiveDataChunkSize(GetColorByteDataOffset() * particlesCount)
    , mTranslationData(nullptr)
    , mRotationSizeData(nullptr)
    , mColorData(nullptr)
{
    ext_assert(particlesCount, "ParticlesRawDataHandler::ctor: particlesCount is zero");
    AllocatePoolMemory();
}

ParticlesRawDataHandler::~ParticlesRawDataHandler()
{
    DeallocatePoolMemory();
}

void ParticlesRawDataHandler::SubTranslationData(const size_t byteDataOffset, const glm::vec3& translation)
{
    ext_assert(mTranslationData, "ParticlesRawDataHandler::SubTranslationData: mTranslationData is null");
    ext_assert(
        byteDataOffset <= (mTranslationDataSize - GetTranslationVectorByteDataOffset()),
        "ParticlesRawDataHandler::SubTranslationData: byteDataOffset out of range");

    float* f_data = (float*)((char*)mTranslationData + byteDataOffset);
    f_data[0] = translation.x;
    f_data[1] = translation.y;
    f_data[2] = translation.z;
}

void ParticlesRawDataHandler::SubRotationSizeData(const size_t byteDataOffset, const float rotation, const float size)
{
    ext_assert(mRotationSizeData, "ParticlesRawDataHandler::SubRotationSizeData: mRotationSizeData is null");
    ext_assert(
        byteDataOffset <= (mRotationSizeDataSize - GetRotationSizeByteDataOffset()),
        "ParticlesRawDataHandler::SubRotationSizeData: byteDataOffset out of range");

    float* f_data = (float*)((char*)mRotationSizeData + byteDataOffset);
    f_data[0] = rotation;
    f_data[1] = size;
}

void ParticlesRawDataHandler::SubColorData(const size_t byteDataOffset, const glm::vec3& color)
{
    ext_assert(mColorData, "ParticlesRawDataHandler::SubColorData: mColorData is null");
    ext_assert(
        byteDataOffset <= (mColorDataSize - GetColorByteDataOffset()),
        "ParticlesRawDataHandler::SubColorData: byteDataOffset out of range");

    float* f_data = (float*)((char*)mColorData + byteDataOffset);
    f_data[0] = color.x;
    f_data[1] = color.y;
    f_data[2] = color.z;
}

void* ParticlesRawDataHandler::CopyToDstActiveTranslationData(void* dst)
{
    return memcpy(dst, mTranslationData, mTranslationActiveDataChunkSize);
}

void* ParticlesRawDataHandler::CopyToDstActiveRotationSizeData(void* dst)
{
    return memcpy(dst, mRotationSizeData, mRotationSizeActiveDataChunkSize);
}

void* ParticlesRawDataHandler::CopyToDstActiveColorData(void* dst)
{
    return memcpy(dst, mColorData, mColorActiveDataChunkSize);
}

void ParticlesRawDataHandler::CopyToMeActiveTranslationData(const void* src, const size_t offset, const size_t byteChunkSize)
{
    ext_assert(
        mTranslationDataSize >= (byteChunkSize - offset),
        "ParticlesRawDataHandler::CopyToMeActiveTranslationData: byteChunkSize and offset out of range");
    (void)memcpy((void*)((char*)mTranslationData + offset), src, byteChunkSize);
}

void ParticlesRawDataHandler::CopyToMeActiveRotationSizeData(const void* src, const size_t offset, const size_t byteChunkSize)
{
    ext_assert(
        mRotationSizeDataSize >= (byteChunkSize - offset),
        "ParticlesRawDataHandler::CopyToMeActiveRotationSizeData: byteChunkSize and offset out of range");
    (void)memcpy((void*)((char*)mRotationSizeData + offset), src, byteChunkSize);
}

void ParticlesRawDataHandler::CopyToMeActiveColorData(const void* src, const size_t offset, const size_t byteChunkSize)
{
    ext_assert(
        mColorDataSize >= (byteChunkSize - offset),
        "ParticlesRawDataHandler::CopyToMeActiveColorData: byteChunkSize and offset out of range");
    (void)memcpy((void*)((char*)mColorData + offset), src, byteChunkSize);
}

void ParticlesRawDataHandler::ResetTranslationData()
{
    memset(mTranslationData, 0, mTranslationDataSize);
}

void ParticlesRawDataHandler::ResetRotationSizeData()
{
    memset(mRotationSizeData, 0, mRotationSizeDataSize);
}

void ParticlesRawDataHandler::ResetColorData()
{
    memset(mColorData, 0, mColorDataSize);
}

void ParticlesRawDataHandler::SetTranslationActiveDataChunkSize(const size_t activeDataChunkSize)
{
    ext_assert(
        activeDataChunkSize <= mTranslationDataSize,
        "ParticlesRawDataHandler::SetTranslationActiveDataChunkSize: activeDataChunkSize out of range");
    mTranslationActiveDataChunkSize = activeDataChunkSize;
}

void ParticlesRawDataHandler::SetRotationSizeActiveDataChunkSize(const size_t activeDataChunkSize)
{
    ext_assert(
        activeDataChunkSize <= mRotationSizeDataSize,
        "ParticlesRawDataHandler::SetRotationSizeActiveDataChunkSize: activeDataChunkSize out of range");
    mRotationSizeActiveDataChunkSize = activeDataChunkSize;
}

void ParticlesRawDataHandler::SetColorActiveDataChunkSize(const size_t activeDataChunkSize)
{
    ext_assert(
        activeDataChunkSize <= mColorDataSize,
        "ParticlesRawDataHandler::SetColorActiveDataChunkSize: activeDataChunkSize out of range");
    mColorActiveDataChunkSize = activeDataChunkSize;
}

size_t ParticlesRawDataHandler::GetTranslationDataSize() const
{
    return mTranslationDataSize;
}

size_t ParticlesRawDataHandler::GetRotationSizeDataSize() const
{
    return mRotationSizeDataSize;
}

size_t ParticlesRawDataHandler::GetColorDataSize() const
{
    return mColorDataSize;
}

size_t ParticlesRawDataHandler::GetTranslationActiveDataChunkSize() const
{
    return mTranslationActiveDataChunkSize;
}

size_t ParticlesRawDataHandler::GetRotationSizeActiveDataChunkSize() const
{
    return mRotationSizeActiveDataChunkSize;
}

size_t ParticlesRawDataHandler::GetColorActiveDataChunkSize() const
{
    return mColorActiveDataChunkSize;
}

void* ParticlesRawDataHandler::GetTranslationData() const
{
    return mTranslationData;
}

void* ParticlesRawDataHandler::GetRotationSizeData() const
{
    return mRotationSizeData;
}

void* ParticlesRawDataHandler::GetColorData() const
{
    return mColorData;
}

void ParticlesRawDataHandler::AllocatePoolMemory()
{
    mTranslationData = malloc(mTranslationDataSize);
    mRotationSizeData = malloc(mRotationSizeDataSize);
    mColorData = malloc(mColorDataSize);
}

void ParticlesRawDataHandler::DeallocatePoolMemory()
{
    if (mTranslationData)
        free(mTranslationData);

    if (mRotationSizeData)
        free(mRotationSizeData);

    if (mColorData)
        free(mColorData);
}
} // namespace EngineCore