#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/GameCore/ShaderImplementation/CubemapShader.h"
#include "Core/GameCore/Components/ComponentData/CubemapComponentData.h"
#include "Core/GraphicsCore/RenderData/CubemapRenderData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class CubemapComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            const CubemapComponentData &mData =
                static_cast<const CubemapComponentData &>(data);

            ShaderParams shaderParams("Cubemap Shader", mData.m_vsShaderPath,
                                      mData.m_fsShaderPath);
            ShaderPool::sharedValue_t shader =
                ShaderPool::GetInstance()
                    ->template GetOrAllocateResource<CubemapShader>(shaderParams);

            CubemapRenderData renderData(shader, mData.m_textureObtainer);

            return std::make_shared<ComponentInstantiationType>(mData, renderData);
        }
    };
}
