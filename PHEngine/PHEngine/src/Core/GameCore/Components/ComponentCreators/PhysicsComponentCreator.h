#pragma once

#include "IComponentCreatable.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"

#include <type_traits>

using namespace EnginePhysics;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class PhysicsComponentCreator
        : public IComponentCreatable
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            const auto &mData = std::static_pointer_cast<PhysicsComponentData>(data);
            mData->mPhysicsDescriptor->Initialize();
            spScene->GetPhysicsWorld()->AddPhysDescriptor(mData->mPhysicsDescriptor);
            return std::make_shared<ComponentInstantiationType>(mData);
        }
    };
}
