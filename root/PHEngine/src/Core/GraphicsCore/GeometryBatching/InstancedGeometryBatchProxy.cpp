#include "InstancedGeometryBatchProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderUtilityFunctions.h"
#include "Core/GraphicsCore/Renderer/ActiveBindedState.h"
#include "Core/GraphicsCore/SceneProxy/InstancedStaticMeshSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/ResourceManagerCore/Pool/InstancedMeshPool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"

using namespace Graphics;
using namespace Graphics::Proxy;
using namespace Resources;

namespace Graphics::GeometryBatching {
InstancedGeometryBatchProxy::InstancedGeometryBatchProxy(const std::shared_ptr<InstancedStaticMeshSceneProxy>& initialSceneProxy)
    : m_renderData(initialSceneProxy->GetRenderData())
    , mBatchKey(initialSceneProxy->GetBatchKey())
    , mIsProxiesRenderOrderChanged(true)
    , mIsProxiesTransformChanged(true)
{
    mInstancedStaticMeshSceneProxies.emplace(initialSceneProxy->GetSceneProxyId(), initialSceneProxy);
}

void InstancedGeometryBatchProxy::Initialize()
{
    const auto shaderIdName = m_renderData.mIsDeferredShaded ? "DeferredNonSkeletalBase Shader" : "ForwardNonSkeletalBase Shader";
    const auto fragmentShaderName = m_renderData.mIsDeferredShaded ? "deferredFS.glsl" : "forwardNoLitFS.glsl";
    ShaderParams shaderParams(shaderIdName);
    shaderParams.SetMainShaders(
        FolderManager::GetInstance()->GetAbsolutePathToRes("simpleVS.glsl"),
        FolderManager::GetInstance()->GetAbsolutePathToRes(fragmentShaderName));

    mShader = std::static_pointer_cast<typename InstancedGeometryBatchProxy::ShaderType>(
        PrimitiveSceneProxy::CreateMaterialShader<InstancedStaticMeshVertexFactory, SimpleShader>(
            "InstancedStaticMeshVertexFactory_SimpleShader_" + m_renderData.mMaterialProxy->MaterialName,
            shaderParams,
            m_renderData.mMaterialProxy));

    MeshPoolParameters poolParameters;
    poolParameters.mModelPath = m_renderData.mModelPath;
    poolParameters.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_skin = InstancedMeshPool::GetInstance()->GetOrAllocateResource(poolParameters);
}

const std::string& InstancedGeometryBatchProxy::GetBatchKey() const
{
    return mBatchKey;
}

void InstancedGeometryBatchProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    PrepareRenderData();
    const auto& shader = GetShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shader->GetShaderName());
    if (needToRebindShader) {
        shader->ExecuteShader();
    }

    shader->GetVertexFactoryShader()->SetMatrices(mCachedWorldMatrices, viewMatrix, projectionMatrix);
    shader->GetMaterialShader()->LoadUniformValues(m_renderData.mMaterialProxy, activeBindedState);
    m_skin->GetBuffer()->RenderInstanced(GL_TRIANGLES, mCachedWorldMatrices.size());
}

void InstancedGeometryBatchProxy::AddInstancedStaticMeshSceneProxy(
    const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy>& sceneProxy)
{
    const auto [insertedIt, inserted] = mInstancedStaticMeshSceneProxies.try_emplace(sceneProxy->GetSceneProxyId(), sceneProxy);
    if (inserted) {
        mIsProxiesRenderOrderChanged = true;
    }
}

void InstancedGeometryBatchProxy::RemoveInstancedStaticMeshSceneProxy(
    const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy>& sceneProxy)
{
    mInstancedStaticMeshSceneProxies.erase(sceneProxy->GetSceneProxyId());
    mIsProxiesRenderOrderChanged = true;
}

std::shared_ptr<InstancedGeometryBatchProxy::ShaderType> InstancedGeometryBatchProxy::GetShader() const
{
    return mShader;
}

std::shared_ptr<IShader> InstancedGeometryBatchProxy::GetBatchShader() const
{
    return mShader;
}

void InstancedGeometryBatchProxy::PrepareRenderData()
{
    if (!mIsProxiesRenderOrderChanged && !mIsProxiesTransformChanged) {
        return;
    }

    mCachedWorldMatrices.clear();
    mCachedWorldMatrices.reserve(mSceneProxiesRenderOrder.size());

    for (const auto sceneProxyId : mSceneProxiesRenderOrder) {
        const auto foundIt = mInstancedStaticMeshSceneProxies.find(sceneProxyId);
        ext_assert(
            foundIt != mInstancedStaticMeshSceneProxies.cend(),
            "InstancedGeometryBatchProxy::PrepareRenderData: Scene proxy not found in batch");
        const auto& proxySp = foundIt->second.lock();
        ext_assert(proxySp, "InstancedGeometryBatchProxy::PrepareRenderData: Scene proxy weak pointer is expired");
        mCachedWorldMatrices.emplace_back(proxySp->GetMatrix());
    }

    mIsProxiesRenderOrderChanged = false;
    mIsProxiesTransformChanged = false;
}

void InstancedGeometryBatchProxy::UpdateValidInstances(std::vector<int32_t> data)
{
    mSceneProxiesRenderOrder = std::move(data);
    mIsProxiesRenderOrderChanged = true;
}

bool InstancedGeometryBatchProxy::IsDeferred() const
{
    return m_renderData.mIsDeferredShaded;
}

void InstancedGeometryBatchProxy::SetIsSlaveTransformDirty(const bool value)
{
    mIsProxiesTransformChanged = value;
}
} // namespace Graphics::GeometryBatching
