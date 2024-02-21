#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/RenderData/StaticMeshRenderData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedMeshPoolParameters.h"

using namespace Graphics::Data;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class RuntimeGeneratedMeshComponentCreator
        : public IComponentCreatable
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            std::shared_ptr<Skin> skin;

            const auto &mData = std::static_pointer_cast<RuntimeGeneratedMeshComponentData>(data);
            assert(eMeshComponentDataType::RUNTIME_GENERATED_MESH == mData->GetMeshComponentDataType());
            RuntimeGeneratedMeshPoolParameters runtimeMeshParams(mData->EngineObjectName, mData->mMaxVerticesCount);

            const auto &materialProxy = mData->m_material->GetMaterialProxyWp().lock();
            assert(materialProxy);
            return std::make_shared<ComponentInstantiationType>(mData, StaticMeshRenderData("", materialProxy, false), runtimeMeshParams);
        }
    };
}
