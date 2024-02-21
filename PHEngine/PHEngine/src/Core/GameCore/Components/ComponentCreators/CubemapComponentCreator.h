#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Components/ComponentData/CubemapComponentData.h"
#include "Core/GraphicsCore/RenderData/CubemapRenderData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class CubemapComponentCreator
        : public IComponentCreatable
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            const auto &mData = std::static_pointer_cast<CubemapComponentData>(data);
            CubemapRenderData renderData(mData->m_textureObtainer);

            return std::make_shared<ComponentInstantiationType>(mData, renderData);
        }
    };
}
