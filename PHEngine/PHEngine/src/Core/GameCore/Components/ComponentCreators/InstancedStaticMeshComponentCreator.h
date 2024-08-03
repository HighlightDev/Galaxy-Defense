#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/GameCore/Components/ComponentData/InstancedMeshComponentData.h"

using namespace Graphics::Data;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class InstancedStaticMeshComponentCreator
        : public IComponentCreatable
    {
        const bool mIsDeferredShaderUsed;

    public:
        InstancedStaticMeshComponentCreator()
            : mIsDeferredShaderUsed(true) // todo: for now only deferred shading is supported for instanced geometry
        {
        }

        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            std::shared_ptr<Skin> skin = nullptr;

            const auto &mData = std::static_pointer_cast<InstancedMeshComponentData>(data);

            const auto &materialProxy = mData->m_material->GetMaterialProxyWp().lock();
            assert(materialProxy);

            return std::make_shared<ComponentInstantiationType>(mData, MeshRenderData(mData->m_pathToMesh, materialProxy, mIsDeferredShaderUsed));
        }
    };
}
