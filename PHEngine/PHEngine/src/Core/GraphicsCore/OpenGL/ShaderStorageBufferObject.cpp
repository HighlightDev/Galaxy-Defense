#include "ShaderStorageBufferObject.h"

#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Graphics::OpenGL {
ShaderStorageBufferObject::ShaderStorageBufferObject(
    const int32_t bindingPoint, const uint32_t flags, const std::vector<uint8_t>& data)
    : BufferObjectBase("ShaderStorageBufferObject", GL_SHADER_STORAGE_BUFFER)
    , mBindingPoint(bindingPoint)
    , m_data(data)
    , m_flags(flags)
{
}

ShaderStorageBufferObject::ShaderStorageBufferObject(const int32_t bindingPoint, const uint32_t flags, const uint32_t dataSize)
    : BufferObjectBase("ShaderStorageBufferObject", GL_SHADER_STORAGE_BUFFER)
    , mBindingPoint(bindingPoint)
    , m_data(dataSize)
    , m_flags(flags)
{
}

ShaderStorageBufferObject::~ShaderStorageBufferObject()
{
}

bool ShaderStorageBufferObject::operator==(const ShaderStorageBufferObject& right) const
{
    return this->m_descriptor == right.m_descriptor && this->mBindingPoint == right.mBindingPoint && this->m_data == right.m_data
        && this->m_flags == right.m_flags;
}

void ShaderStorageBufferObject::SendDataToGPU()
{
    GenBuffer();
    m_allocatedBufferSize = m_data.size() * sizeof(uint8_t);
    glNamedBufferStorage(m_descriptor, m_allocatedBufferSize, m_data.size() ? m_data.data() : nullptr, m_flags);
#ifdef _DEBUG
    const GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::string errorMsg;
        switch (err) {
        case GL_INVALID_ENUM:
            errorMsg = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorMsg = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorMsg = "GL_INVALID_OPERATION";
            break;
        default:
            errorMsg = "Unknown error";
        }
        LogInfo("ShaderStorageBufferObject::SendDataToGPU: OpenGL error msg: ", errorMsg);
    }
#endif
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

void ShaderStorageBufferObject::GenBuffer()
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
        "ShaderStorageBufferObject::GenBuffer must be called from Render thread");
    glCreateBuffers(1, &m_descriptor);
}

void ShaderStorageBufferObject::BindBuffer() const
{
    glBindBufferBase(m_bufferTarget, mBindingPoint, static_cast<GLuint>(m_descriptor));

#ifdef _DEBUG
    const GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::string errorMsg;
        switch (err) {
        case GL_INVALID_ENUM:
            errorMsg = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorMsg = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorMsg = "GL_INVALID_OPERATION";
            break;
        default:
            errorMsg = "Unknown error";
        }
        LogInfo("ShaderStorageBufferObject::BindSSBO: OpenGL error msg: ", errorMsg);
    }
#endif
}

void ShaderStorageBufferObject::BufferSubData(const size_t offset, const size_t size, const void* data) const
{
    ext_assert(
        m_allocatedBufferSize >= size,
        "Buffer size exceeds allocated buffer size, available allocated: " + std::to_string(m_allocatedBufferSize)
            + ", requested size: " + std::to_string(size));
    glNamedBufferSubData(m_descriptor, offset, size, data);
}

void ShaderStorageBufferObject::CopyFromBuffer(
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
    glCopyNamedBufferSubData(sourceBufferId, m_descriptor, sourceOffset, destOffset, size);
    glMemoryBarrier(static_cast<GLbitfield>(barrierBit));
}

std::vector<uint32_t> ShaderStorageBufferObject::GetBufferStorageFlags() const
{
    std::vector<uint32_t> flags;
    glBindBuffer(m_bufferTarget, m_descriptor);
    GLint flagsValue;
    glGetNamedBufferParameteriv(m_descriptor, GL_BUFFER_STORAGE_FLAGS, &flagsValue);
    if (flagsValue & GL_MAP_READ_BIT) {
        flags.push_back(GL_MAP_READ_BIT);
    }
    if (flagsValue & GL_MAP_WRITE_BIT) {
        flags.push_back(GL_MAP_WRITE_BIT);
    }
    if (flagsValue & GL_DYNAMIC_STORAGE_BIT) {
        flags.push_back(GL_DYNAMIC_STORAGE_BIT);
    }
    return flags;
}
} // namespace Graphics::OpenGL
