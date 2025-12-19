#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ElectricBeamComponentData.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"
#include "IComponentCreatable.h"

using namespace Graphics::Data;
using namespace Resources;

namespace EngineCore {
class Scene;
template<typename ComponentInstantiationType>
class ElectricBeamComponentCreator : public IComponentCreatable {
public:
    virtual
        typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        const auto& mData = std::static_pointer_cast<ElectricBeamComponentData>(data);

        const auto& materialProxy = mData->m_material->GetMaterialProxyWp().lock();
        ext_assert(materialProxy, "ElectricBeamComponentCreator::CreateComponent: materialProxy is null");

        auto component = std::make_shared<ComponentInstantiationType>(
            mData->EngineObjectName,
            MeshRenderData("", materialProxy, false),
            RuntimeGeneratedMeshPoolParameters(mData->EngineObjectName, 0, 0),
            mData->mIsEnabled,
            mData->mIsVisible);

        // Set properties from data
        component->SetStartWorldPosition(mData->StartPoint);
        component->SetEndWorldPosition(mData->EndPoint);
        component->SetBeamThickness(mData->BeamThickness);
        component->SetBeamCount(mData->BeamCount);
        component->SetJitterAmount(mData->JitterAmount);
        component->SetUpdateFrequency(mData->UpdateFrequency);

        return component;
    }
};

} // namespace EngineCore
