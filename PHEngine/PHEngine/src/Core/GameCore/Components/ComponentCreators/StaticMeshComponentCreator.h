#pragma once

#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "IComponentCreatable.h"

#include <type_traits>

using namespace Graphics::Data;

namespace EngineCore {
class Scene;

template<typename ComponentInstantiationType>
class StaticMeshComponentCreator : public IComponentCreatable {
    const bool mIsDeferredShaderUsed;

public:
    StaticMeshComponentCreator(const bool useDeferredShader)
        : mIsDeferredShaderUsed(useDeferredShader)
    {
    }

    virtual
        typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        std::shared_ptr<Skin> skin = nullptr;

        const auto& mData = std::static_pointer_cast<MeshComponentData>(data);
        assert(eMeshComponentDataType::STATIC_OR_SKELETAL_MESH == mData->GetMeshComponentDataType());

        const auto& materialProxy = mData->m_material->GetMaterialProxyWp().lock();
        assert(materialProxy);

        return std::make_shared<ComponentInstantiationType>(
            mData, MeshRenderData(mData->m_pathToMesh, materialProxy, mIsDeferredShaderUsed));
    }
};
} // namespace EngineCore
