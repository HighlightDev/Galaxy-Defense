#include "UniformBufferControlBlock.h"

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace Graphics::OpenGL {

void CheckGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        const char* error;
        switch (err) {
        case GL_INVALID_ENUM:
            error = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "GL_OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "Unknown Error";
            break;
        }
        std::cerr << "OpenGL error " << error << " (" << err << "), at " << fname << ":" << line << " - for " << stmt
                  << std::endl;
        err = glGetError();
    }
}

#define GL_CHECK(stmt)                                                                                                           \
    do {                                                                                                                         \
        stmt;                                                                                                                    \
        CheckGLError(#stmt, __FILE__, __LINE__);                                                                                 \
    } while (0)

UniformBufferControlBlock::UniformBufferControlBlock(
    const std::string& blockName, const uint32_t bindingPoint, const uint32_t initialBufferSize)
    : mUniformBufferSize(initialBufferSize)
    , mBlockName(blockName)
    , mBindingPoint(bindingPoint)
{
    glGenBuffers(1, &mUniformBufferDescriptorId);
    glBindBuffer(GL_UNIFORM_BUFFER, mUniformBufferDescriptorId);
    glBufferData(GL_UNIFORM_BUFFER, initialBufferSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

    glGenBuffers(1, &mUniformBufferDescriptorId);
    glBindBuffer(GL_UNIFORM_BUFFER, mUniformBufferDescriptorId);
    glBufferData(GL_UNIFORM_BUFFER, mUniformBufferSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
    assert(mTotalMemoryAllocated <= mUniformBufferSize);
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
    assert(blockIndex != GL_INVALID_INDEX);
    glUniformBlockBinding(shaderProgramId, blockIndex, mBindingPoint);
    glUseProgram(0);
}

void UniformBufferControlBlock::SetDataInUniformBuffer(const UniformBufferUserInfo& userInfo, const void* data, const size_t size)
{
    assert(mUniformBufferDescriptorId != std::numeric_limits<uint32_t>::max());
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mUniformBufferDescriptorId, userInfo.mMemoryOffset, userInfo.mMemorySize);
    glNamedBufferSubData(mUniformBufferDescriptorId, userInfo.mMemoryOffset, userInfo.mMemorySize, data);
}
} // namespace Graphics::OpenGL