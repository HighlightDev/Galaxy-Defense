#pragma once

#include <stdint.h>

#include <cstdint>
#include <limits>

#undef min // without it conflicts with math min, max function
#undef max

namespace Graphics::OpenGL {

struct UniformBufferUserInfo {

    uint32_t mUniformBufferUserId{std::numeric_limits<uint32_t>::max()};

    uint32_t shaderProgramId{std::numeric_limits<uint32_t>::max()};

    uint32_t mMemoryOffset{0};

    uint32_t mMemorySize{0};
};
} // namespace Graphics::OpenGL