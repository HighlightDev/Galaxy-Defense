#pragma once
#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GraphicsCore/RenderData/BillboardRenderData.h"
#include "Core/IoCore/FolderManager.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Resources;
using namespace IO;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class BillboardComponentCreator
        : public IComponentCreatable
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            const auto mData = std::static_pointer_cast<BillboardComponentData>(data);
            const auto &materialProxy = mData->m_material->GetMaterialProxyWp().lock();
            assert(materialProxy);
            return std::make_shared<ComponentInstantiationType>(mData, BillboardRenderData(materialProxy));
        }
    };
}
