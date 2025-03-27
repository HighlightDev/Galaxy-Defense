#pragma once

#include "UniformBufferUserInfo.h"

#include <stdint.h>

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace Graphics::OpenGL {

class UniformBufferControlBlock {

private:
    std::string mBlockName;

    uint32_t mBindingPoint{0};

    uint32_t mUniformBufferUsersCount{0};

    uint32_t mUniformBufferSize{0};

    uint32_t mUniformBufferDescriptorId{std::numeric_limits<uint32_t>::max()};

    std::vector<UniformBufferUserInfo> mUniformBufferUsers;

    uint32_t mTotalMemoryAllocated{0};

public:
    UniformBufferControlBlock(const std::string& blockName, const uint32_t bindingPoint, const uint32_t initialBufferSize);

    ~UniformBufferControlBlock();

    void CleanUp();

    UniformBufferUserInfo AllocateMemoryInUniformBuffer(const uint32_t shaderProgramId, const uint32_t memorySize);

    uint32_t GetUniformBufferDescriptorId() const;

    void SetDataInUniformBuffer(const UniformBufferUserInfo& userInfo, const void* data);

    void SetDataInUniformBuffer(const UniformBufferUserInfo& userInfo, const void* data, const size_t offset, const size_t size);

private:
    void ReallocateBuffer(const uint32_t newMemorySize);

    void BindUniformBlockToBindingPoint(const uint32_t shaderProgramId);
};
} // namespace Graphics::OpenGL