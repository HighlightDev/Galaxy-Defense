#pragma once

#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/RenderData/SkyboxRenderData.h"
#include "IComponentCreatable.h"

#include <type_traits>

using namespace Graphics::Data;

namespace EngineCore {
class Scene;

template<typename ComponentInstantiationType>
class SkyboxComponentCreator : public IComponentCreatable {
public:
    virtual
        typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        const auto& mData = std::static_pointer_cast<SkyboxComponentData>(data);

        const auto& materialProxy = mData->m_material->GetMaterialProxyWp().lock();
        assert(materialProxy);

        return std::make_shared<ComponentInstantiationType>(mData, SkyboxRenderData(materialProxy));
    }
};
} // namespace EngineCore
