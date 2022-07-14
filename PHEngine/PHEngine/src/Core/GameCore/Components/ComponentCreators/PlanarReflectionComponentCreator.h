#pragma once

#include "IComponentCreatable.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"

#include <type_traits>

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class PlanarReflectionComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<PlanarReflectionComponent, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
           const PlanarReflectionComponentData &mData =
                static_cast<const PlanarReflectionComponentData &>(data);

            assert(mData.m_ownerCamera);

            const auto &component = std::make_shared<ComponentInstantiationType>(mData);

            mData.m_ownerCamera->SetPlanarReflectionComponent(component);
            return component;
        }
    };
}
