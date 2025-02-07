#pragma once
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/ResourceManagerCore/Policy/RuntimeGeneratedMeshAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"
#include "PoolBase.h"

using namespace EngineCore;

namespace Resources {
class RuntimeGeneratedMeshPool : public PoolBase<Skin, RuntimeGeneratedMeshPoolParameters, RuntimeGeneratedMeshAllocationPolicy> {
    static std::unique_ptr<RuntimeGeneratedMeshPool> m_instance;

public:
    using poolType_t = PoolBase<Skin, RuntimeGeneratedMeshPoolParameters, RuntimeGeneratedMeshAllocationPolicy>;

    std::string ToString() const override;

    static std::unique_ptr<RuntimeGeneratedMeshPool>& GetInstance();

    static void ReloadInstance();
};

} // namespace Resources
