#pragma once
#include "BufferObjectBase.h"
#include "Core/GameCore/LoggerExtension.h"
#include "DataCarryFlag.h"

#include <gl/glew.h>
#include <stdint.h>

#include <cstddef>
#include <memory>
#include <vector>

using namespace EngineCore;

namespace Graphics::OpenGL {
template<typename DataType, int32_t buffer_usage = GL_DYNAMIC_DRAW>
class CompositeVertexBufferObject : public BufferObjectBase {
    struct GLDataLayout {
        int32_t vertexAttribIndex;
        int32_t glType;
        int32_t vectorSize;
        int32_t offset;
    };

protected:
    std::vector<DataType> m_rawData;
    std::vector<GLDataLayout> m_glData;
    int32_t m_stride;
    int32_t m_countOfIndices;
    size_t m_reservedElementCount{0};

public:
    CompositeVertexBufferObject(const int32_t bufferTarget, const int32_t stride)
        : BufferObjectBase("VertexPosition", bufferTarget)
        , m_stride(stride)
    {
    }

    ~CompositeVertexBufferObject() override = default;

    void AddData(const int32_t vertexAttribIndex, const int32_t glType, const int32_t vectorSize, const int32_t offset)
    {
        GLDataLayout newData;
        newData.vertexAttribIndex = vertexAttribIndex;
        newData.glType = glType;
        newData.vectorSize = vectorSize;
        newData.offset = offset;
        m_glData.emplace_back(std::move(newData));
    }

    void SetRawData(const std::vector<DataType>& data)
    {
        m_rawData = data;
    }

    void SetRawData(std::vector<DataType>&& data)
    {
        m_rawData = std::move(data);
    }

    // Reserve GPU capacity (in DataType elements) so the buffer is allocated empty in SendDataToGPU and
    // streamed later via BufferSubData — symmetric to the size-based VertexBufferObject constructor. The
    // vertex attribute layout is still configured. Ignored when raw data is provided instead.
    void ReserveCapacity(const size_t elementCount)
    {
        m_reservedElementCount = elementCount;
    }

public:
    std::vector<DataType>& GetCastedDataRef()
    {
        return m_rawData;
    }

    size_t GetElementByteSize() const override
    {
        return sizeof(DataType);
    }

    void SendDataToGPU() override
    {
        GenBuffer();
        BindBuffer();

        // Allocate from raw data when provided, otherwise allocate an empty buffer of the reserved size
        // (to be streamed later via BufferSubData).
        const size_t elementCount = m_rawData.empty() ? m_reservedElementCount : m_rawData.size();
        ext_assert(
            elementCount > 0,
            "CompositeVertexBufferObject::SendDataToGPU: nothing to allocate (provide raw data or ReserveCapacity)");
        m_countOfIndices = static_cast<int32_t>(elementCount);
        m_allocatedBufferSize = elementCount * sizeof(DataType);

        glBufferData(m_bufferTarget, m_allocatedBufferSize, m_rawData.empty() ? nullptr : m_rawData.data(), buffer_usage);
        for (const auto& data : m_glData) {

            void* m_offset = reinterpret_cast<void*>(static_cast<uintptr_t>(data.offset));
            glEnableVertexAttribArray(data.vertexAttribIndex);
            if (data.glType == GL_INT || data.glType == GL_UNSIGNED_BYTE || data.glType == GL_UNSIGNED_INT) {
                glVertexAttribIPointer(data.vertexAttribIndex, data.vectorSize, data.glType, m_stride, m_offset);
            } else if (data.glType == GL_DOUBLE) {
                glVertexAttribLPointer(data.vertexAttribIndex, data.vectorSize, data.glType, m_stride, m_offset);
            } else if (data.glType == GL_FLOAT) {
                glVertexAttribPointer(data.vertexAttribIndex, data.vectorSize, data.glType, GL_FALSE, m_stride, m_offset);
            }
            glVertexAttribDivisor(data.vertexAttribIndex, 0);
        }

        m_rawData.clear();

        UnbindBuffer();
    }

    size_t GetCountOfIndices() const override
    {
        return m_countOfIndices;
    }

    size_t GetTotalLengthOfData() const override
    {
        return m_allocatedBufferSize;
    }

    size_t GetVectorSize() const override
    {
        return 0; // Not applicable for composite VBO, as it can contain multiple data types with different vector sizes
    }

    size_t GetVertexAttribIndex() const override
    {
        return 0; // Not applicable for composite VBO, as it can contain multiple data types with different vertex attrib indices
    }

    void CleanUp() override
    {
        UnbindBuffer();
        glDeleteBuffers(1, &m_descriptor);
    }
};
} // namespace Graphics::OpenGL
