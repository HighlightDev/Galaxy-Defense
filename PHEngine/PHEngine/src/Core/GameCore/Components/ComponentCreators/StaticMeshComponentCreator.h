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
        const bool mIsDeferredShaderUsed;

    public:
        StaticMeshComponentCreator(const bool useDeferredShader)
            : mIsDeferredShaderUsed(useDeferredShader)
        {
        }

        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            std::shared_ptr<Skin> skin = nullptr;

            const auto &mData = std::static_pointer_cast<MeshComponentData>(data);
            assert(eMeshComponentDataType::STATIC_OR_SKELETAL_MESH == mData->GetMeshComponentDataType());

            const auto &materialProxy = mData->m_material->GetMaterialProxyWp().lock();
            assert(materialProxy);

            const auto shaderIdName = mIsDeferredShaderUsed ? "DeferredNonSkeletalBase Shader" : "ForwardNonSkeletalBase Shader";
            const auto fragmentShaderName = mIsDeferredShaderUsed ? "deferredFS.glsl" : "forwardFS.glsl";
            const ShaderParams shaderParams(
                shaderIdName,
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + "simpleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() +
                    "composite_shaders" + SLASH + fragmentShaderName);

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
                                                                StaticMeshRenderData(mData->m_pathToMesh,
                                                                                     staticMeshShader,
                                                                                     planarReflectionShader,
                                                                                     materialProxy, mIsDeferredShaderUsed));
        }
    };
}
