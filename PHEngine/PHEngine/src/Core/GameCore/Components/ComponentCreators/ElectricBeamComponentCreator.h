#pragma once

#include "Core/GameCore/Components/ComponentData/ElectricBeamComponentData.h"
#include "Core/GameCore/Components/ElectricBeamComponent.h"

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

        auto component = std::make_shared<ComponentInstantiationType>(mData->EngineObjectName);

        // Set properties from data
        component->SetStartPoint(mData->StartPoint);
        component->SetEndPoint(mData->EndPoint);
        component->SetBeamColor(mData->BeamColor);
        component->SetBeamThickness(mData->BeamThickness);
        component->SetBeamCount(mData->BeamCount);
        component->SetJitterAmount(mData->JitterAmount);
        component->SetUpdateFrequency(mData->UpdateFrequency);
        component->SetIsActive(mData->IsActive);

        return component;
    }
};

} // namespace EngineCore
