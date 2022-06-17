#pragma once
#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GraphicsCore/RenderData/BillboardRenderData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Resources;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class BillboardComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
             const BillboardComponentData &mData =
                static_cast<const BillboardComponentData &>(data);

            int32_t primitive = (int32_t)SimplePrimitiveType::POINT;
            SimplePrimitivePool::sharedValue_t skin =
                SimplePrimitivePool::GetInstance()->GetOrAllocateResource(primitive);
            typename TexturePool::sharedValue_t texture =
                TexturePool::GetInstance()->GetOrAllocateResource(
                    mData.m_pathToTexture);
            ShaderParams shaderParams("Billboard Shader", mData.m_vsShaderPath,
                                      mData.m_fsShaderPath, mData.m_gsShaderPath);
            ShaderPool::sharedValue_t shader =
                ShaderPool::GetInstance()
                    ->template GetOrAllocateResource<BillboardShader>(shaderParams);

            BillboardRenderData renderData(skin, shader, texture);

            return std::make_shared<ComponentInstantiationType>(mData, renderData);
        }
    };
}
