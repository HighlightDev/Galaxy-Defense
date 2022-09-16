#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkyboxVertexFactory.h"
#include "Core/GraphicsCore/RenderData/SkyboxRenderData.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class SkyboxComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            const SkyboxComponentData &mData =
                static_cast<const SkyboxComponentData &>(data);

            auto skin =
                SimplePrimitivePool::GetInstance()->GetOrAllocateResource(static_cast<int32_t>(SimplePrimitiveType::INVERTED_VERTICES_DIRECTION_CUBE));

            const auto &materialProxy = spScene->RegisterMaterialInstance(std::shared_ptr<IMaterial>(mData.m_material));

            const ShaderParams shaderParams(
                "SkyboxForwardShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t skyboxMeshShader =
                CreateMaterialShader<SkyboxVertexFactory, SimpleShader>(
                    "SkyboxVertexFactory_SimpleShader_" + materialProxy->MaterialName,
                    shaderParams, materialProxy);

            const ShaderParams planarReflectionParams(
                "PlanarReflectionShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "planarReflectionVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
                CreateMaterialShader<SkyboxVertexFactory, CapturePlanarReflectionShader>("SkyboxVertexFactory_CapturePlanarReflectionShader_" + materialProxy->MaterialName,
                                                                                         planarReflectionParams, materialProxy);

            return std::make_shared<ComponentInstantiationType>(
                mData, SkyboxRenderData(skin, skyboxMeshShader, planarReflectionShader, materialProxy));
        }
    };
}
