#include "ShaderStorageBufferObject.h"

namespace Graphics::OpenGL {
ShaderStorageBufferObject::ShaderStorageBufferObject(const int32_t bindingPoint, const std::vector<uint8_t>& data)
    : BufferObjectBase("ShaderStorageBufferObject", GL_SHADER_STORAGE_BUFFER)
    , mBindingPoint(bindingPoint)
    , m_data(data)
{
}

ShaderStorageBufferObject::ShaderStorageBufferObject(const int32_t bindingPoint, const uint32_t dataSize)
    : BufferObjectBase("ShaderStorageBufferObject", GL_SHADER_STORAGE_BUFFER)
    , mBindingPoint(bindingPoint)
    , m_data(dataSize)
{
}

ShaderStorageBufferObject::~ShaderStorageBufferObject()
{
}

void ShaderStorageBufferObject::SendDataToGPU()
{
    GenBuffer();
    BindBuffer();
    m_allocatedBufferSize = m_data.size() * sizeof(uint8_t);
    glBufferData(m_bufferTarget, m_allocatedBufferSize, m_data.size() ? m_data.data() : nullptr, GL_DYNAMIC_DRAW);
    m_data.clear();
}

size_t ShaderStorageBufferObject::GetCountOfIndices() const
{
    // For SSBO this method is not applicable
    return 0;
}

size_t ShaderStorageBufferObject::GetTotalLengthOfData() const
{
    // For SSBO this method is not applicable
    return 0;
}

size_t ShaderStorageBufferObject::GetVectorSize() const
{
    // For SSBO this method is not applicable
    return 0;
}

size_t ShaderStorageBufferObject::GetVertexAttribIndex() const
{
    // For SSBO this method is not applicable
    return 0;
}

size_t ShaderStorageBufferObject::GetElementByteSize() const
{
    // For SSBO this method is not applicable
    return 0;
}

void ShaderStorageBufferObject::CleanUp()
{
    UnbindBuffer();
    glDeleteBuffers(1, &m_descriptor);
}

void ShaderStorageBufferObject::BindSSBO() const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, mBindingPoint, static_cast<GLuint>(m_descriptor));
}
} // namespace Graphics::OpenGL
