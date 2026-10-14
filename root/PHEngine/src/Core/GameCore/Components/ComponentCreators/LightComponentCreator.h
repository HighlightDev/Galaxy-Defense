#pragma once

#include "Core/GameCore/Components/ComponentData/LightComponentData.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "IComponentCreatable.h"

#include <type_traits>

using namespace Graphics;

namespace EngineCore {
class Scene;

template<typename ComponentInstantiationType>
class LightComponentCreator : public IComponentCreatable {
public:
    virtual
        typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        const auto& mData = std::static_pointer_cast<LightComponentData>(data);
        if (mData->ShadowInfo) {
            mData->ShadowInfo->Initialize();
        }
        return std::make_shared<ComponentInstantiationType>(mData);
    }
};
} // namespace EngineCore
