#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ShaderType.h"

#include <string>

namespace Graphics {
namespace OpenGL {
class BufferObjectBase {
protected:
    GLuint m_descriptor;

    GLint m_bufferTarget;

    std::string mAttribArrayIndexName;

    size_t m_allocatedBufferSize{0};

public:
    BufferObjectBase(const std::string& attribArrayIndexName, const GLint bufferTarget = GL_ARRAY_BUFFER);

    virtual ~BufferObjectBase();

    virtual void GenBuffer();

    virtual void BindBuffer() const;

    virtual void UnbindBuffer() const;

    virtual void SendDataToGPU() = 0;

    virtual size_t GetCountOfIndices() const = 0;

    virtual size_t GetTotalLengthOfData() const = 0;

    virtual size_t GetVectorSize() const = 0;

    virtual size_t GetVertexAttribIndex() const = 0;

    virtual size_t GetElementByteSize() const = 0;

    virtual void BufferSubData(const size_t offset, const size_t size, const void* data) const;

    virtual void CleanUp() = 0;

    GLuint GetDescriptor() const;

    std::string GetAttribArrayIndexName() const;

    GLint GetBufferTarget() const;

    size_t GetAllocatedBufferSize() const;

    virtual void CopyFromBuffer(
        const GLuint sourceBufferId,
        const size_t sourceOffset,
        const size_t destOffset,
        const size_t size,
        const eMemoryBarrierType barrierBit) const;

    virtual std::vector<uint32_t> GetBufferStorageFlags() const;

    size_t GetElementByteSizeForGLType(const int32_t glType) const;
};
} // namespace OpenGL
} // namespace Graphics
