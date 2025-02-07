#pragma once

#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "IComponentCreatable.h"

#include <type_traits>

using namespace Graphics::Data;

namespace EngineCore {
class Scene;

template<typename ComponentInstantiationType>
class SkeletalMeshComponentCreator : public IComponentCreatable {
public:
    virtual
        typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        const auto& mData = std::static_pointer_cast<MeshComponentData>(data);
        assert(eMeshComponentDataType::STATIC_OR_SKELETAL_MESH == mData->GetMeshComponentDataType());

        const auto& materialProxy = mData->m_material->GetMaterialProxyWp().lock();
        assert(materialProxy);

        MeshRenderData renderData(mData->m_pathToMesh, materialProxy, true);
        return std::make_shared<ComponentInstantiationType>(mData, renderData);
    }
};
} // namespace EngineCore
