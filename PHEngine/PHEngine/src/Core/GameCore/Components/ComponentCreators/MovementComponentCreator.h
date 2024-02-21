#pragma once

#include "IComponentCreatable.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"

#include <type_traits>

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class MovementComponentCreator
        : public IComponentCreatable
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            const auto &mData = std::static_pointer_cast<MovementComponentData>(data);
            return std::make_shared<ComponentInstantiationType>(mData);
        }
    };
}
