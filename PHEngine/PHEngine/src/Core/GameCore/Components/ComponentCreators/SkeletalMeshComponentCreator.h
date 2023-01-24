#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/SkeletalMeshVertexFactory.h"
#include "Core/GraphicsCore/RenderData/SkeletalMeshRenderData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class SkeletalMeshComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            const MeshComponentData &mData = static_cast<const MeshComponentData &>(data);
            assert(eMeshComponentDataType::STATIC_OR_SKELETAL_MESH == mData.GetMeshComponentDataType());

            const auto &materialProxy = spScene->RegisterMaterialInstance(std::shared_ptr<IMaterial>(mData.m_material));

            const ShaderParams shaderParams(
                "DeferredNonSkeletalBase Shader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "deferredFS.glsl");

            typename CompositeShaderPool::sharedValue_t skeletalMeshShader =
                CreateMaterialShader<SkeletalMeshVertexFactory<4>, SimpleShader>(
                    "SkeletalMeshVertexFactory<4>_SimpleShader_" + materialProxy->MaterialName,
                    shaderParams, materialProxy);

            const ShaderParams planarReflectionParams(
                "PlanarReflectionShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "planarReflectionVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
                CreateMaterialShader<SkeletalMeshVertexFactory<4>,
                                     CapturePlanarReflectionShader>(
                    "SkeletalMeshVertexFactory<4>_CapturePlanarReflectionShader" + materialProxy->MaterialName,
                    planarReflectionParams, materialProxy);

            SkeletalMeshRenderData renderData(mData.m_pathToMesh, skeletalMeshShader,
                                              planarReflectionShader, materialProxy);

            return std::make_shared<ComponentInstantiationType>(
                mData,
                renderData);
        }
    };
}
