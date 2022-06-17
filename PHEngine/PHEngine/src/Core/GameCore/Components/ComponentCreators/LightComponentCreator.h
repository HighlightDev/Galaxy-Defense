#pragma once

#include "IComponentCreatable.h"
#include "Core/GameCore/Components/ComponentData/LightComponentData.h"

#include <type_traits>

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class LightComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            const LightComponentData &mData = static_cast<const LightComponentData &>(data);
            return std::make_shared<ComponentInstantiationType>(mData);
        }
    };
}
