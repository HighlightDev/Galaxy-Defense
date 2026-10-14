#pragma once

#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/InstancedStaticMeshVertexFactory.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

#include <glm/mat4x4.hpp>

#include <string>
#include <unordered_map>
#include <vector>

using namespace Graphics::Data;
using namespace EngineCore::ShaderImpl;
using namespace Graphics::Mesh;
using namespace Graphics::Proxy;

namespace Graphics::Proxy {
class InstancedStaticMeshSceneProxy;
}

namespace Graphics {
class ActiveBindedState;
}

namespace Graphics::GeometryBatching {
class InstancedGeometryBatchProxy {
    using ShaderType = VertexFactoryMaterialCompositeShader<InstancedStaticMeshVertexFactory, SimpleShader>;

    MeshRenderData m_renderData;

    std::shared_ptr<ShaderType> mShader;

    std::shared_ptr<Skin> m_skin;

    std::string mBatchKey; // model name + material name

    std::unordered_map<int32_t /*scene proxy id*/, std::weak_ptr<InstancedStaticMeshSceneProxy>> mInstancedStaticMeshSceneProxies;

    std::vector<glm::mat4> mCachedWorldMatrices;

    std::vector<int32_t /*scene proxy id*/> mSceneProxiesRenderOrder;

    bool mIsProxiesRenderOrderChanged;

    bool mIsProxiesTransformChanged;

public:
    explicit InstancedGeometryBatchProxy(
        const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy>& initialSceneProxy);

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ::Graphics::ActiveBindedState& activeBindedState);

    void AddInstancedStaticMeshSceneProxy(const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy>& sceneProxy);

    void RemoveInstancedStaticMeshSceneProxy(const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy>& sceneProxy);

    void Initialize();

    const std::string& GetBatchKey() const;

    void UpdateValidInstances(std::vector<int32_t> data);

    std::shared_ptr<IShader> GetBatchShader() const;

    bool IsDeferred() const;

    void SetIsSlaveTransformDirty(const bool value);

private:
    std::shared_ptr<ShaderType> GetShader() const;

    void PrepareRenderData();
};
} // namespace Graphics::GeometryBatching
