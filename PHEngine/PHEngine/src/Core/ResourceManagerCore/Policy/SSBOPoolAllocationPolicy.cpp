#include "SSBOPoolAllocationPolicy.h"

#include "Core/GraphicsCore/OpenGL/ShaderStorageBufferObject.h"

namespace Resources {

std::shared_ptr<Graphics::OpenGL::ShaderStorageBufferObject>
SSBOPoolAllocationPolicy::AllocateMemory(const SSBOPoolParameters& params)
{
    return std::make_shared<Graphics::OpenGL::ShaderStorageBufferObject>(
        params.bindingPoint, params.flags, params.bytesToAllocate);
}

void SSBOPoolAllocationPolicy::DeallocateMemory(const std::shared_ptr<Graphics::OpenGL::ShaderStorageBufferObject>& value)
{
    value->CleanUp();
}

} // namespace Resources