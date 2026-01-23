#pragma once

#include <gl/glew.h>
#include <stdint.h>

#include <cstddef>
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

    void GenBuffer();

    void BindBuffer() const;

    void UnbindBuffer() const;

    virtual void SendDataToGPU() = 0;

    virtual size_t GetCountOfIndices() const = 0;

    virtual size_t GetTotalLengthOfData() const = 0;

    virtual size_t GetVectorSize() const = 0;

    virtual size_t GetVertexAttribIndex() const = 0;

    virtual size_t GetElementByteSize() const = 0;

    virtual void BufferSubData(const size_t offset, const size_t size, const void* data) const;

    virtual void CleanUp() = 0;

    virtual void* GetData();

    GLuint GetDescriptor() const;

    std::string GetAttribArrayIndexName() const;

    GLint GetBufferTarget() const;

    size_t GetAllocatedBufferSize() const;
};
} // namespace OpenGL
} // namespace Graphics
