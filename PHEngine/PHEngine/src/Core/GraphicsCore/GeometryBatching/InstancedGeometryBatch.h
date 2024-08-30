#pragma once

#include <glm/mat4x4.hpp>
#include <string>
#include <vector>

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/InstancedStaticMeshVertexFactory.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

using namespace Graphics::Data;
using namespace EngineCore::ShaderImpl;
using namespace Graphics::Mesh;
using namespace Graphics::Proxy;

namespace Graphics::Proxy
{
    class InstancedStaticMeshSceneProxy;
}

namespace Graphics::GeometryBatching
{
    class InstancedGeometryBatch
    {
        using ShaderType = VertexFactoryMaterialCompositeShader<InstancedStaticMeshVertexFactory, SimpleShader>;

        MeshRenderData m_renderData;

        std::shared_ptr<ShaderType> mShader;

        std::shared_ptr<Skin> m_skin;

        std::string mBatchKey; // model name + material name

        std::vector<std::weak_ptr<InstancedStaticMeshSceneProxy>> mInstancedStaticMeshSceneProxies;

        std::vector<glm::mat4> mCachedWorldMatrices;

    public:
        explicit InstancedGeometryBatch(const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy> &initialSceneProxy);

        void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy,
                    const glm::mat4 &viewMatrix,
                    const glm::mat4 &projectionMatrix);

        void AddInstancedStaticMeshSceneProxy(const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy> &sceneProxy);

        void RemoveInstancedStaticMeshSceneProxy(const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy> &sceneProxy);

        void Initialize();

        std::string GetBatchKey() const;

        bool IsProxyActive(const int32_t sceneProxyId) const;

        // the instance id in order for rendering
        int32_t GetInstanceId(const int32_t sceneProxyId) const;
    private:
        bool IsProxyActive(const std::shared_ptr<InstancedStaticMeshSceneProxy>& sceneProxy) const;

        std::shared_ptr<ShaderType> GetShader() const;

        std::vector<glm::mat4> CollectAllWorldMatrices();
    };
}
