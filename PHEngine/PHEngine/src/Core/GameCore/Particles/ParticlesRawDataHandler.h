#pragma once

#include <cstdint>
#include <stdint.h>
#include <glm/vec3.hpp>

namespace EngineCore
{
    class ParticlesRawDataHandler
    {
        void *mTranslationData;
        size_t mTranslationDataSize;
        size_t mTranslationActiveDataChunkSize;

        void *mRotationSizeData;
        size_t mRotationSizeDataSize;
        size_t mRotationSizeActiveDataChunkSize;

    public:
        ParticlesRawDataHandler(const size_t particlesCount);

        ~ParticlesRawDataHandler();
        
        void SubTranslationData(const size_t byteDataOffset, const glm::vec3& translation);

        void SetTranslationActiveDataChunkSize(const size_t activeDataChunkSize);

        size_t GetTranslationDataSize() const;

        size_t GetTranslationActiveDataChunkSize() const;

        void* GetTranslationData() const;

        void* CopyToDstActiveTranslationData(void* dst);

        void CopyToMeActiveTranslationData(const void* src, const size_t offset, const size_t byteChunkSize);

        inline static size_t GetTranslationVectorByteDataOffset()
        {
            return sizeof(float) * 3;
        }

        void SubRotationSizeData(const size_t byteDataOffset, const float rotation, const float size);

        void SetRotationSizeActiveDataChunkSize(const size_t activeDataChunkSize);

        size_t GetRotationSizeDataSize() const;

        size_t GetRotationSizeActiveDataChunkSize() const;

        void* GetRotationSizeData() const;

        void* CopyToDstActiveRotationSizeData(void* dst);

        void CopyToMeActiveRotationSizeData(const void* src, const size_t offset, const size_t byteChunkSize);

        inline static size_t GetRotationSizeByteDataOffset()
        {
            return sizeof(float) * 2;
        }

    private:
        void AllocatePoolMemory();

        void DeallocatePoolMemory();
    };
}
