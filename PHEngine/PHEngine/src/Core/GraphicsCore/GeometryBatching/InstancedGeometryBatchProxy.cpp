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
{
    mInstancedStaticMeshSceneProxies.emplace_back(initialSceneProxy);
}

void InstancedGeometryBatchProxy::Initialize()
{
    const auto shaderIdName = m_renderData.mIsDeferredShaded ? "DeferredNonSkeletalBase Shader" : "ForwardNonSkeletalBase Shader";
    const auto fragmentShaderName = m_renderData.mIsDeferredShaded ? "deferredFS.glsl" : "forwardNoLitFS.glsl";
    const ShaderParams shaderParams(
        shaderIdName,
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "simpleVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + fragmentShaderName);

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

std::string InstancedGeometryBatchProxy::GetBatchKey() const
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
    const auto alreadyContains = std::any_of(
        mInstancedStaticMeshSceneProxies.cbegin(), mInstancedStaticMeshSceneProxies.cend(), [sceneProxy](const auto& proxyWp) {
            if (const auto& proxySp = proxyWp.lock()) {
                return proxySp->GetSceneProxyId() == sceneProxy->GetSceneProxyId();
            }
            return false;
        });

    if (!alreadyContains) {
        mInstancedStaticMeshSceneProxies.emplace_back(sceneProxy);
    }
}

void InstancedGeometryBatchProxy::RemoveInstancedStaticMeshSceneProxy(
    const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy>& sceneProxy)
{
    auto removeIt = std::remove_if(
        mInstancedStaticMeshSceneProxies.begin(), mInstancedStaticMeshSceneProxies.end(), [sceneProxy](const auto& proxyWp) {
            if (const auto& proxySp = proxyWp.lock()) {
                return proxySp->GetSceneProxyId() == sceneProxy->GetSceneProxyId();
            }
            return false;
        });
    mInstancedStaticMeshSceneProxies.erase(removeIt);
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
    mCachedWorldMatrices.clear();
    mCachedWorldMatrices.reserve(mInstancedStaticMeshSceneProxies.size());

    for (const auto sceneProxyId : mSceneProxiesRenderOrder) {
        const auto foundIt = std::find_if(
            mInstancedStaticMeshSceneProxies.cbegin(),
            mInstancedStaticMeshSceneProxies.cend(),
            [sceneProxyId](const auto& proxyWp) {
                if (const auto& proxySp = proxyWp.lock()) {
                    return proxySp->GetSceneProxyId() == sceneProxyId;
                }
                return false;
            });
        ext_assert(
            foundIt != mInstancedStaticMeshSceneProxies.cend(),
            "InstancedGeometryBatchProxy::PrepareRenderData: Scene proxy not found in batch");
        const auto& proxySp = foundIt->lock();
        ext_assert(proxySp, "InstancedGeometryBatchProxy::PrepareRenderData: Scene proxy weak pointer is expired");
        mCachedWorldMatrices.emplace_back(proxySp->GetMatrix());
    }
}

void InstancedGeometryBatchProxy::UpdateValidInstances(const std::vector<int32_t>& data)
{
    mSceneProxiesRenderOrder = data;
}
} // namespace Graphics::GeometryBatching
