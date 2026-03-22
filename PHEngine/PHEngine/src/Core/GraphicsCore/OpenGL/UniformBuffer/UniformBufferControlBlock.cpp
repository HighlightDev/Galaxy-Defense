#include "UniformBufferControlBlock.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace EngineCore;

namespace Graphics::OpenGL {

UniformBufferControlBlock::UniformBufferControlBlock(
    const std::string& blockName, const uint32_t bindingPoint, const uint32_t initialBufferSize)
    : mUniformBufferSize(initialBufferSize)
    , mBlockName(blockName)
    , mBindingPoint(bindingPoint)
{
    glCreateBuffers(1, &mUniformBufferDescriptorId);
    glNamedBufferData(mUniformBufferDescriptorId, initialBufferSize, nullptr, GL_DYNAMIC_DRAW);
    GLint bufferSize = 0;
    glGetNamedBufferParameteriv(mUniformBufferDescriptorId, GL_BUFFER_SIZE, &bufferSize);
    if (bufferSize != initialBufferSize) {
        LogInfo("ERROR: UniformBufferControlBlock: glBufferData failed to allocate the requested buffer size for ", mBlockName);
    }
}

UniformBufferControlBlock::~UniformBufferControlBlock()
{
}

uint32_t UniformBufferControlBlock::GetUniformBufferDescriptorId() const
{
    return mUniformBufferDescriptorId;
}

void UniformBufferControlBlock::ReallocateBuffer(const uint32_t newMemorySize)
{
    if (mUniformBufferDescriptorId != std::numeric_limits<uint32_t>::max()) {
        glDeleteBuffers(1, &mUniformBufferDescriptorId);

        mTotalMemoryAllocated = 0;
        if (mUniformBufferUsersCount > 0) {
            for (const auto& userInfo : mUniformBufferUsers) {
                BindUniformBlockToBindingPoint(userInfo.shaderProgramId);
                mTotalMemoryAllocated += userInfo.mMemorySize;
            }
        }
    }
    glCreateBuffers(1, &mUniformBufferDescriptorId);
    glNamedBufferData(mUniformBufferDescriptorId, mUniformBufferSize, nullptr, GL_DYNAMIC_DRAW);

    mUniformBufferSize = newMemorySize;
}

void UniformBufferControlBlock::CleanUp()
{
    mUniformBufferUsers.clear();
    mUniformBufferUsersCount = 0;
    glDeleteBuffers(1, &mUniformBufferDescriptorId);
}

UniformBufferUserInfo
UniformBufferControlBlock::AllocateMemoryInUniformBuffer(const uint32_t shaderProgramId, const uint32_t memorySize)
{
    ext_assert(
        mTotalMemoryAllocated <= mUniformBufferSize,
        "UniformBufferControlBlock::AllocateMemoryInUniformBuffer: Total memory allocated exceeds buffer size");
    const auto leftMemory = mUniformBufferSize - mTotalMemoryAllocated;
    if (memorySize <= leftMemory) {
        // take memory from buffer
        UniformBufferUserInfo userInfo{mUniformBufferUsersCount, shaderProgramId, mTotalMemoryAllocated, memorySize};
        mUniformBufferUsers.emplace_back(userInfo);
        BindUniformBlockToBindingPoint(shaderProgramId);
        ++mUniformBufferUsersCount;
        mTotalMemoryAllocated += memorySize;
        return userInfo;
    } else {
        ReallocateBuffer(static_cast<uint32_t>(mUniformBufferSize * 1.5f));
        return AllocateMemoryInUniformBuffer(shaderProgramId, memorySize);
    }
}

void UniformBufferControlBlock::BindUniformBlockToBindingPoint(const uint32_t shaderProgramId)
{
    glUseProgram(shaderProgramId);
    const uint32_t blockIndex = glGetUniformBlockIndex(shaderProgramId, mBlockName.c_str());
    glUniformBlockBinding(shaderProgramId, blockIndex, mBindingPoint);
    glUseProgram(0);
}

void UniformBufferControlBlock::SetDataInUniformBuffer(const UniformBufferUserInfo& userInfo, const void* data, const size_t size)
{
    ext_assert(
        mUniformBufferDescriptorId != std::numeric_limits<uint32_t>::max(),
        "UniformBufferControlBlock::SetDataInUniformBuffer: Uniform buffer descriptor ID is invalid");
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mUniformBufferDescriptorId, userInfo.mMemoryOffset, userInfo.mMemorySize);
    glNamedBufferSubData(mUniformBufferDescriptorId, userInfo.mMemoryOffset, size, data);
}

void UniformBufferControlBlock::SetDataInUniformBuffer(
    const UniformBufferUserInfo& userInfo, const void* data, const size_t offset, const size_t size)
{
    ext_assert(
        mUniformBufferDescriptorId != std::numeric_limits<uint32_t>::max(),
        "UniformBufferControlBlock::SetDataInUniformBuffer: Uniform buffer descriptor ID is invalid");
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mUniformBufferDescriptorId, userInfo.mMemoryOffset + offset, size);
    glNamedBufferSubData(mUniformBufferDescriptorId, userInfo.mMemoryOffset + offset, size, data);
}

void UniformBufferControlBlock::BindUniformBufferRange(
    const UniformBufferUserInfo& userInfo, const uint32_t offset, const uint32_t size)
{
    ext_assert(
        mUniformBufferDescriptorId != std::numeric_limits<uint32_t>::max(),
        "UniformBufferControlBlock::BindUniformBufferRange: Uniform buffer descriptor ID is invalid");
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mUniformBufferDescriptorId, userInfo.mMemoryOffset + offset, size);
}

void UniformBufferControlBlock::BindUniformBuffer()
{
    ext_assert(
        mUniformBufferDescriptorId != std::numeric_limits<uint32_t>::max(),
        "UniformBufferControlBlock::BindUniformBuffer: Uniform buffer descriptor ID is invalid");
    glBindBufferBase(GL_UNIFORM_BUFFER, mBindingPoint, mUniformBufferDescriptorId);
}
} // namespace Graphics::OpenGL