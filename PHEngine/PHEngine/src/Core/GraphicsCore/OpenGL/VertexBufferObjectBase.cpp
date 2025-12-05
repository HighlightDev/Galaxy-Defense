#include "VertexBufferObjectBase.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"

namespace Graphics {
namespace OpenGL {
VertexBufferObjectBase::VertexBufferObjectBase(const std::string& attribArrayIndexName, const int32_t bufferTarget)
    : m_bufferTarget(bufferTarget)
    , mAttribArrayIndexName(attribArrayIndexName)
{
}

VertexBufferObjectBase::~VertexBufferObjectBase()
{
}

void VertexBufferObjectBase::GenBuffer()
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
        "GenBuffer must be called from Render thread");
    glGenBuffers(1, &m_descriptor);
}

void VertexBufferObjectBase::BindVBO()
{
    glBindBuffer(m_bufferTarget, m_descriptor);
}

void VertexBufferObjectBase::UnbindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void* VertexBufferObjectBase::GetData()
{
    return nullptr;
}

size_t VertexBufferObjectBase::GetDescriptor() const
{
    return m_descriptor;
}

std::string VertexBufferObjectBase::GetAttribArrayIndexName() const
{
    return mAttribArrayIndexName;
}

void VertexBufferObjectBase::BufferSubData(const size_t offset, const size_t size, const void* data) const
{
    ext_assert(m_allocatedBufferSize >= size, "Buffer size exceeds allocated buffer size");
    glBufferSubData(m_bufferTarget, offset, size, data);
}

int32_t VertexBufferObjectBase::GetBufferTarget() const
{
    return m_bufferTarget;
}

size_t VertexBufferObjectBase::GetAllocatedBufferSize() const
{
    return m_allocatedBufferSize;
}
} // namespace OpenGL
} // namespace Graphics
