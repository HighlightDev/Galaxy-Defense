#include "BufferObjectBase.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/ThreadHelper.h"

namespace Graphics::OpenGL {
BufferObjectBase::BufferObjectBase(const std::string& attribArrayIndexName, const GLint bufferTarget)
    : m_bufferTarget(bufferTarget)
    , mAttribArrayIndexName(attribArrayIndexName)
{
}

BufferObjectBase::~BufferObjectBase()
{
}

void BufferObjectBase::GenBuffer()
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName),
        "GenBuffer must be called from Render thread");
    glGenBuffers(1, &m_descriptor);
}

void BufferObjectBase::BindBuffer() const
{
    glBindBuffer(m_bufferTarget, m_descriptor);
}

void BufferObjectBase::UnbindBuffer() const
{
    glBindBuffer(m_bufferTarget, 0);
}

GLuint BufferObjectBase::GetDescriptor() const
{
    return m_descriptor;
}

std::string BufferObjectBase::GetAttribArrayIndexName() const
{
    return mAttribArrayIndexName;
}

void BufferObjectBase::BufferSubData(const size_t offset, const size_t size, const void* data) const
{
    ext_assert(
        m_allocatedBufferSize >= size,
        "Buffer size exceeds allocated buffer size, available allocated: " + std::to_string(m_allocatedBufferSize)
            + ", requested size: " + std::to_string(size));
    glBindBuffer(m_bufferTarget, m_descriptor);
    glBufferSubData(m_bufferTarget, offset, size, data);
    glBindBuffer(m_bufferTarget, 0);
}

GLint BufferObjectBase::GetBufferTarget() const
{
    return m_bufferTarget;
}

size_t BufferObjectBase::GetAllocatedBufferSize() const
{
    return m_allocatedBufferSize;
}

void BufferObjectBase::CopyFromBuffer(
    const GLuint sourceBufferId,
    const size_t sourceOffset,
    const size_t destOffset,
    const size_t size,
    const eMemoryBarrierType barrierBit) const
{
    ext_assert(
        m_allocatedBufferSize >= destOffset + size,
        "Destination buffer size is too small for copy operation, available allocated: " + std::to_string(m_allocatedBufferSize)
            + ", required size: " + std::to_string(destOffset + size));
    glBindBuffer(m_bufferTarget, m_descriptor);
    glCopyBufferSubData(sourceBufferId, m_descriptor, sourceOffset, destOffset, size);
    glBindBuffer(m_bufferTarget, 0);
    glMemoryBarrier(static_cast<GLbitfield>(barrierBit));
}

std::vector<uint32_t> BufferObjectBase::GetBufferStorageFlags() const
{
    return {};
}

size_t BufferObjectBase::GetElementByteSizeForGLType(const int32_t glType) const
{
    switch (glType) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return 2;
    case GL_INT:
    case GL_UNSIGNED_INT:
        return 4;
    case GL_FLOAT:
        return 4;
    case GL_DOUBLE:
        return 8;
    default:
        ext_assert(false, "Unsupported GL type");
        return 0;
    }
}
} // namespace Graphics::OpenGL
