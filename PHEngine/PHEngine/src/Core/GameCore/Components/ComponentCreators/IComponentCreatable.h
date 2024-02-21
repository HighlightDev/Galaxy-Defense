#pragma once

#include <memory>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"

using namespace Resources;

namespace EngineCore
{
    class Scene;
    class Component;

    class IComponentCreatable
    {
    public:
        virtual std::shared_ptr<Component> CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const = 0;
    };
}