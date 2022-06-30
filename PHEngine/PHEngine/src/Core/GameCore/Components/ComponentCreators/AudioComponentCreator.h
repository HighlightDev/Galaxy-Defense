#pragma once

#include "IComponentCreatable.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"

#include <type_traits>

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class AudioComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<SoundComponent, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            return std::make_shared<ComponentInstantiationType>(data);
        }
    };
}
