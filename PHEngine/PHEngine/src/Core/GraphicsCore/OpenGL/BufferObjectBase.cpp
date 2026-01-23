#include "BufferObjectBase.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"

namespace Graphics {
namespace OpenGL {
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
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
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

void* BufferObjectBase::GetData()
{
    return nullptr;
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
} // namespace OpenGL
} // namespace Graphics
