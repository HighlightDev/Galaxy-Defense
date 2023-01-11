#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/RenderData/StaticMeshRenderData.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/IoCore/FolderManager.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;
using namespace IO;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class StaticMeshComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            std::shared_ptr<Skin> skin = nullptr;

            const MeshComponentData &mData = static_cast<const MeshComponentData &>(data);

            if (eMeshComponentDataType::STATIC_OR_SKELETAL_MESH == mData.GetMeshComponentDataType())
            {
                skin = MeshPool::GetInstance()->GetOrAllocateResource(mData.m_pathToMesh);
            }
            else if (eMeshComponentDataType::SIMPLE_MESH == mData.GetMeshComponentDataType())
            {
                const SimpleMeshComponentData &simpleMeshData = static_cast<const SimpleMeshComponentData &>(mData);
                if ("PLANE" == simpleMeshData.mSimpleMeshType)
                {
                    skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource((int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES);
                }
            }

            assert(skin);

            const auto &materialProxy = spScene->RegisterMaterialInstance(std::shared_ptr<IMaterial>(mData.m_material));

            const ShaderParams shaderParams(
                "DeferredNonSkeletalBase Shader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "deferredFS.glsl");

            typename CompositeShaderPool::sharedValue_t staticMeshShader =
                CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
                    "StaticMeshVertexFactory_SimpleShader_" + materialProxy->MaterialName, shaderParams, materialProxy);

            const ShaderParams planarReflectionParams(
                "PlanarReflectionShader",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "planarReflectionVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "forwardFS.glsl");

            typename CompositeShaderPool::sharedValue_t planarReflectionShader =
                CreateMaterialShader<StaticMeshVertexFactory,
                                     CapturePlanarReflectionShader>(
                    "StaticMeshVertexFactory_CapturePlanarReflectionShader_" + materialProxy->MaterialName, planarReflectionParams, materialProxy);

            return std::make_shared<ComponentInstantiationType>(mData,
                                                                StaticMeshRenderData(skin,
                                                                                     staticMeshShader,
                                                                                     planarReflectionShader,
                                                                                     materialProxy, true));
        }
    };
}
