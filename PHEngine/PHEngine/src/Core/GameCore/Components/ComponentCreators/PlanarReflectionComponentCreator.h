#pragma once

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "IComponentCreatable.h"

#include <type_traits>

namespace EngineCore {
class Scene;

template<typename ComponentInstantiationType>
class PlanarReflectionComponentCreator : public IComponentCreatable {
public:
    virtual typename std::
        enable_if<std::is_base_of<PlanarReflectionComponent, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        const auto& mData = std::static_pointer_cast<PlanarReflectionComponentData>(data);

        const auto& ownerCameraSp = mData->m_ownerCamera.lock();
        assert(ownerCameraSp);

        const auto& component = std::make_shared<ComponentInstantiationType>(mData);

        ownerCameraSp->SetPlanarReflectionComponent(component);
        return component;
    }
};
} // namespace EngineCore
