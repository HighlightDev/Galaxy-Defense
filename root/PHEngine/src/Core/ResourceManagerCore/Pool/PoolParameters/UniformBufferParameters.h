#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace Graphics::OpenGL;

namespace Resources {
struct UniformBufferParameters {
    std::string mUserName;
    std::string mBlockName;
    uint32_t mBindingPoint;
    uint32_t mMemorySize;
    uint32_t mShaderProgramId;

    friend struct std::hash<UniformBufferParameters>;

    bool operator==(const UniformBufferParameters& other) const
    {
        return this->mUserName == other.mUserName;
    }
};
} // namespace Resources

namespace std {
using namespace Resources;
template<>
struct hash<UniformBufferParameters> {
    std::size_t operator()(const UniformBufferParameters& params) const
    {
        return hash<std::string>()(params.mUserName);
    }
};
} // namespace std
