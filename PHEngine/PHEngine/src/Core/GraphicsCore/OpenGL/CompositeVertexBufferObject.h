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

        ext_assert(not m_rawData.empty(), "CompositeVertexBufferObject::SendDataToGPU: No data to send to GPU");
        m_countOfIndices = static_cast<int32_t>(m_rawData.size());
        m_allocatedBufferSize = m_rawData.size() * sizeof(DataType);

        glBufferData(m_bufferTarget, m_allocatedBufferSize, m_rawData.data(), buffer_usage);
        for (const auto& data : m_glData) {

            void* m_offset = reinterpret_cast<void*>(data.offset);
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
