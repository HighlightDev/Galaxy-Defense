#pragma once

#include "Core/GraphicsCore/OpenGL/ShaderStorageBufferObject.h"
#include "Core/ResourceManagerCore/Policy/SSBOPoolAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/SSBOPoolParameters.h"
#include "PoolBase.h"

namespace Resources {

class SSBOPool : public PoolBase<Graphics::OpenGL::ShaderStorageBufferObject, SSBOPoolParameters, SSBOPoolAllocationPolicy> {
    static std::unique_ptr<SSBOPool> m_instance;

public:
    using poolType_t = PoolBase<Graphics::OpenGL::ShaderStorageBufferObject, SSBOPoolParameters, SSBOPoolAllocationPolicy>;

    std::string ToString() const override;

    static std::unique_ptr<SSBOPool>& GetInstance();

    static void ReloadInstance();
};
} // namespace Resources
