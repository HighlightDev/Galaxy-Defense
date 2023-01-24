#pragma once
#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
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
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            const BillboardComponentData &mData = static_cast<const BillboardComponentData &>(data);

            typename TexturePool::sharedValue_t texture = TexturePool::GetInstance()->GetOrAllocateResource(mData.m_pathToTexture);
            const ShaderParams shaderParams(
                "Billboard Shader",
                FolderManager::GetInstance()->GetShadersPath() + "billboardVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() + "billboardFS.glsl",
                FolderManager::GetInstance()->GetShadersPath() + "billboardGS.glsl");

            const auto &shader = ShaderPool::GetInstance()->template GetOrAllocateResource<BillboardShader>(shaderParams);

            BillboardRenderData renderData(shader, texture);

            return std::make_shared<ComponentInstantiationType>(mData, renderData);
        }
    };
}
