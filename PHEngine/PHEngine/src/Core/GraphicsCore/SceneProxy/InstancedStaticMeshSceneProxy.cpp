#include "InstancedStaticMeshSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchProxy.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatchRenderer.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;
using namespace Graphics::GeometryBatching;

namespace Graphics {
namespace Proxy {
InstancedStaticMeshSceneProxy::InstancedStaticMeshSceneProxy(const InstancedStaticMeshComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , m_renderData(component->GetRenderData())
{
}

InstancedStaticMeshSceneProxy::~InstancedStaticMeshSceneProxy()
{
}

void InstancedStaticMeshSceneProxy::PostConstructorInitialize()
{
    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        const auto& batcherRendererSp = deferredShadingSceneRendererSp->GetInstancedGeometryBatchRenderer();

        if (batcherRendererSp->CheckIfBatchProxyExists(GetBatchKey())) {
            const auto& batchProxy = batcherRendererSp->GetBatchProxy(GetBatchKey());
            batchProxy->AddInstancedStaticMeshSceneProxy(shared_from_this());
        } else {
            const auto& batchProxy = std::make_shared<InstancedGeometryBatchProxy>(shared_from_this());
            const bool bSuccess = batcherRendererSp->TryToAddBatchProxy(batchProxy);
            ext_assert(bSuccess, "InstancedStaticMeshSceneProxy::PostConstructorInitialize: Failed to add batch proxy");
            batchProxy->Initialize();
            mBatchProxy = batchProxy;
        }
    }
}

void InstancedStaticMeshSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    // this code should not be invoked
    ext_assert(false, "InstancedStaticMeshSceneProxy::Render: This method should not be invoked");
}

void InstancedStaticMeshSceneProxy::RenderPlanarReflection(
    const glm::vec4& plane,
    const glm::mat4& mirrorMatrix,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    // this code should not be invoked
    ext_assert(false, "InstancedStaticMeshSceneProxy::RenderPlanarReflection: This method should not be invoked");
}

bool InstancedStaticMeshSceneProxy::IsDeferred() const
{
    return m_renderData.mIsDeferredShaded;
}

bool InstancedStaticMeshSceneProxy::IsFrustumCullTestNeeded() const
{
    // all proxies will be rendered as instanced geometry
    return false;
}

ePrimitiveProxyType InstancedStaticMeshSceneProxy::GetPrimitiveProxyType() const
{
    return ePrimitiveProxyType::INDIRECT_RENDERED_PROXY;
}

MeshRenderData InstancedStaticMeshSceneProxy::GetRenderData() const
{
    return m_renderData;
}

std::string InstancedStaticMeshSceneProxy::GetBatchKey() const
{
    return m_renderData.mModelPath + "_" + m_renderData.mMaterialProxy->MaterialName + "_deferred_"
        + std::to_string(m_renderData.mIsDeferredShaded);
}

RenderInfo InstancedStaticMeshSceneProxy::GetRenderInfo() const
{
    ext_assert(mBatchProxy, "InstancedStaticMeshSceneProxy::GetRenderInfo: Batch proxy is null");
    return RenderInfo{mBatchProxy->GetBatchShader()->GetShaderName()};
}

} // namespace Proxy
} // namespace Graphics
