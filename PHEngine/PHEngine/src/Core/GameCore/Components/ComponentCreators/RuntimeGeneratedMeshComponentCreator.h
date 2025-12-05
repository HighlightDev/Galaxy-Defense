#pragma once

#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"
#include "IComponentCreatable.h"

#include <type_traits>

using namespace Resources;
using namespace Graphics::Data;

namespace EngineCore {
class Scene;

template<typename ComponentInstantiationType>
class RuntimeGeneratedMeshComponentCreator : public IComponentCreatable {
public:
    virtual
        typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        std::shared_ptr<Skin> skin;

        const auto& mData = std::static_pointer_cast<RuntimeGeneratedMeshComponentData>(data);
        ext_assert(
            eMeshComponentDataType::RUNTIME_GENERATED_MESH == mData->GetMeshComponentDataType(),
            "Invalid mesh component data type, expected RUNTIME_GENERATED_MESH");
        RuntimeGeneratedMeshPoolParameters runtimeMeshParams(mData->EngineObjectName, mData->mMaxVerticesCount, 0);

        const auto& materialProxy = mData->m_material->GetMaterialProxyWp().lock();
        ext_assert(materialProxy, "Material proxy is null for runtime generated mesh component");
        return std::make_shared<ComponentInstantiationType>(mData, MeshRenderData("", materialProxy, false), runtimeMeshParams);
    }
};
} // namespace EngineCore
