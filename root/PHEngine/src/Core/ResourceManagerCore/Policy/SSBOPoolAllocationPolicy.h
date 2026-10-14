#pragma once

#include "Core/ResourceManagerCore/Pool/PoolParameters/SSBOPoolParameters.h"

#include <memory>

namespace Graphics::OpenGL {
class ShaderStorageBufferObject;
}

namespace Resources {
class SSBOPoolAllocationPolicy {
public:
    static std::shared_ptr<Graphics::OpenGL::ShaderStorageBufferObject> AllocateMemory(const SSBOPoolParameters& params);

    static void DeallocateMemory(const std::shared_ptr<Graphics::OpenGL::ShaderStorageBufferObject>& value);
};
} // namespace Resources
