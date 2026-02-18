#pragma once

#include "BufferObjectBase.h"

#include <vector>

namespace Graphics::OpenGL {
class ShaderStorageBufferObject : public BufferObjectBase {

    GLuint mBindingPoint;

    std::vector<uint8_t> m_data;

    GLbitfield m_flags;

public:
    explicit ShaderStorageBufferObject(const int32_t bindingPoint, const uint32_t flags, const std::vector<uint8_t>& data);

    explicit ShaderStorageBufferObject(const int32_t bindingPoint, const uint32_t flags, const uint32_t dataSize);

    ~ShaderStorageBufferObject() override;

    void GenBuffer() override;

    void SendDataToGPU() override;

    size_t GetCountOfIndices() const override;

    size_t GetTotalLengthOfData() const override;

    size_t GetVectorSize() const override;

    size_t GetVertexAttribIndex() const override;

    size_t GetElementByteSize() const override;

    void CleanUp() override;

    void BindBuffer() const override;

    void BufferSubData(const size_t offset, const size_t size, const void* data) const override;

    void CopyFromBuffer(
        const GLuint sourceBufferId,
        const size_t sourceOffset,
        const size_t destOffset,
        const size_t size,
        const eMemoryBarrierType barrierBit) const override;

    std::vector<uint32_t> GetBufferStorageFlags() const override;
};
} // namespace Graphics::OpenGL
