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
        size_t mActiveDataChunkSize;

       /* void *mTranslationData;
        size_t mTranslationDataSize;
        size_t mActiveDataChunkSize;

        void *mTranslationData;
        size_t mTranslationDataSize;
        size_t mActiveDataChunkSize;*/

    public:
        ParticlesRawDataHandler(const size_t dataSize);

        ~ParticlesRawDataHandler();
        
        void SubTranslationData(const size_t byteDataOffset, const glm::vec3& translation);

        void SetActiveDataChunkSize(const size_t activeDataChunkSize);

        size_t GetTranslationDataSize() const;

        size_t GetActiveDataChunkSize() const;

        void* GetTranslationData() const;

        void* CopyToDstActiveTranslationData(void* dst);

        void CopyToMeActiveTranslationData(const void* src, const size_t offset, const size_t byteChunkSize);

        inline static size_t GetTranslationVectorByteDataOffset()
        {
            return sizeof(float) * 3;
        }

    private:
        void AllocatePoolMemory(const size_t dataSize);

        void DeallocatePoolMemory();
    };
}
