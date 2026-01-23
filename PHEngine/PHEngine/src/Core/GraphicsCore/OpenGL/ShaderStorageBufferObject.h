#pragma once

#include "BufferObjectBase.h"

#include <vector>

namespace Graphics::OpenGL {
class ShaderStorageBufferObject : public BufferObjectBase {

    GLuint mBindingPoint;

    std::vector<uint8_t> m_data;

public:
    explicit ShaderStorageBufferObject(const int32_t bindingPoint, const std::vector<uint8_t>& data);

    explicit ShaderStorageBufferObject(const int32_t bindingPoint, const uint32_t dataSize);

    ~ShaderStorageBufferObject() override;

    void SendDataToGPU() override;

    size_t GetCountOfIndices() const override;

    size_t GetTotalLengthOfData() const override;

    size_t GetVectorSize() const override;

    size_t GetVertexAttribIndex() const override;

    size_t GetElementByteSize() const override;

    void CleanUp() override;

    void BindSSBO() const;
};
} // namespace Graphics::OpenGL
