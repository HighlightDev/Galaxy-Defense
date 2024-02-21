#pragma once

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/GameCore/Components/ComponentData/ForwardShadingMeshComponentData.h"
#include "Core/GraphicsCore/RenderData/ForwardShadingMeshRenderData.h"

using namespace Graphics::Data;

#include <type_traits>

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class ForwardShadingMeshComponentCreator
        : public IComponentCreatable
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            const auto &mData = std::static_pointer_cast<ForwardShadingMeshComponentData>(data);

            const auto &materialProxy = mData->m_material->GetMaterialProxyWp().lock();
            assert(materialProxy);
            return std::make_shared<ComponentInstantiationType>(mData, ForwardShadingMeshRenderData(mData->mPathToMesh, materialProxy));
        }
    };
}
