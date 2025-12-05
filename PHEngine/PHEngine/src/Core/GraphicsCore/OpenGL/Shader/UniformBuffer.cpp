#include "UniformBuffer.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/OpenGL/UniformBuffer/UniformBufferControlBlock.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace Graphics::OpenGL {

UniformBuffer::UniformBuffer(
    const std::shared_ptr<UniformBufferControlBlock>& controlBlock, const UniformBufferUserInfo& userInfo)
    : mControlBlock(controlBlock)
    , mUserInfo(userInfo)
{
}

void UniformBuffer::SetDataExplicit(const void* data, const size_t size, const size_t offset)
{
    ext_assert(mControlBlock, "UniformBuffer::SetDataExplicit: Control block is null");
    ext_assert(offset + size <= mUserInfo.mMemorySize, "UniformBuffer::SetDataExplicit: Data range exceeds buffer size");
    ext_assert(
        mUserInfo.mUniformBufferUserId != std::numeric_limits<uint32_t>::max(),
        "UniformBuffer::SetDataExplicit: Invalid uniform buffer user ID");
    mControlBlock->SetDataInUniformBuffer(mUserInfo, data, offset, size);
}

void UniformBuffer::SetDataExplicit(const void* data, const size_t size)
{
    ext_assert(mControlBlock, "UniformBuffer::SetDataExplicit: Control block is null");
    ext_assert(
        mUserInfo.mUniformBufferUserId != std::numeric_limits<uint32_t>::max(),
        "UniformBuffer::SetDataExplicit: Invalid uniform buffer user ID");
    ext_assert(size <= mUserInfo.mMemorySize, "UniformBuffer::SetDataExplicit: Data range exceeds buffer size");
    mControlBlock->SetDataInUniformBuffer(mUserInfo, data, size);
}

void UniformBuffer::ResetBuffer()
{
    if (mControlBlock) {
        mControlBlock->CleanUp();
        mControlBlock.reset();
    }
}

void UniformBuffer::BindUniformBuffer()
{
    mControlBlock->BindUniformBuffer();
}

void UniformBuffer::BindUniformBufferRange(const uint32_t offset, const uint32_t size)
{
    ext_assert(mControlBlock, "UniformBuffer::BindUniformBufferRange: Control block is null");
    ext_assert(offset + size <= mUserInfo.mMemorySize, "UniformBuffer::BindUniformBufferRange: Data range exceeds buffer size");
    ext_assert(
        mUserInfo.mUniformBufferUserId != std::numeric_limits<uint32_t>::max(),
        "UniformBuffer::BindUniformBufferRange: Invalid uniform buffer user ID");
    mControlBlock->BindUniformBufferRange(mUserInfo, offset, size);
}

UniformBuffer UniformBuffer::CreateUniformBuffer(
    const std::shared_ptr<UniformBufferControlBlock>& controlBlock,
    const GLuint shaderProgram,
    const GLsizeiptr size,
    bool dynamic)
{
    const auto& userInfo = controlBlock->AllocateMemoryInUniformBuffer(shaderProgram, size);
    return UniformBuffer(controlBlock, userInfo);
}

uint32_t UniformBuffer::GetUniformBufferUserId() const
{
    return mUserInfo.mUniformBufferUserId;
}

} // namespace Graphics::OpenGL
