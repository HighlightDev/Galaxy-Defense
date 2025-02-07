#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdint.h>

#include <cstdint>

namespace EngineCore {
class ParticlesRawDataHandler {
    size_t mTranslationDataSize;
    size_t mTranslationActiveDataChunkSize;
    size_t mRotationSizeDataSize;
    size_t mRotationSizeActiveDataChunkSize;
    size_t mColorDataSize;
    size_t mColorActiveDataChunkSize;

    void* mTranslationData;
    void* mRotationSizeData;
    void* mColorData;

public:
    ParticlesRawDataHandler(const size_t particlesCount);

    ~ParticlesRawDataHandler();

    void SubTranslationData(const size_t byteDataOffset, const glm::vec3& translation);
    void SubRotationSizeData(const size_t byteDataOffset, const float rotation, const float size);
    void SubColorData(const size_t byteDataOffset, const glm::vec4& color);

    void SetTranslationActiveDataChunkSize(const size_t activeDataChunkSize);
    void SetRotationSizeActiveDataChunkSize(const size_t activeDataChunkSize);
    void SetColorActiveDataChunkSize(const size_t activeDataChunkSize);

    size_t GetTranslationDataSize() const;
    size_t GetRotationSizeDataSize() const;
    size_t GetColorDataSize() const;

    size_t GetTranslationActiveDataChunkSize() const;
    size_t GetRotationSizeActiveDataChunkSize() const;
    size_t GetColorActiveDataChunkSize() const;

    void* GetTranslationData() const;
    void* GetRotationSizeData() const;
    void* GetColorData() const;

    void* CopyToDstActiveTranslationData(void* dst);
    void* CopyToDstActiveRotationSizeData(void* dst);
    void* CopyToDstActiveColorData(void* dst);

    void CopyToMeActiveTranslationData(const void* src, const size_t offset, const size_t byteChunkSize);
    void CopyToMeActiveRotationSizeData(const void* src, const size_t offset, const size_t byteChunkSize);
    void CopyToMeActiveColorData(const void* src, const size_t offset, const size_t byteChunkSize);

    void ResetTranslationData();
    void ResetRotationSizeData();
    void ResetColorData();

    inline static size_t GetTranslationVectorByteDataOffset()
    {
        return sizeof(float) * 3;
    }

    inline static size_t GetRotationSizeByteDataOffset()
    {
        return sizeof(float) * 2;
    }

    inline static size_t GetColorByteDataOffset()
    {
        return sizeof(float) * 4;
    }

private:
    void AllocatePoolMemory();

    void DeallocatePoolMemory();
};
} // namespace EngineCore
