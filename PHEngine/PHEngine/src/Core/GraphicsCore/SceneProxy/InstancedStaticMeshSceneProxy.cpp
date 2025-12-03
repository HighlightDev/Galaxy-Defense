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
            assert(bSuccess);
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
    assert(false);
}

void InstancedStaticMeshSceneProxy::RenderPlanarReflection(
    const glm::vec4& plane,
    const glm::mat4& mirrorMatrix,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    // this code should not be invoked
    assert(false);
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
    return m_renderData.mModelPath + "_" + m_renderData.mMaterialProxy->MaterialName;
}

RenderInfo InstancedStaticMeshSceneProxy::GetRenderInfo() const
{
    assert(mBatchProxy);
    return RenderInfo{mBatchProxy->GetBatchShader()->GetShaderName()};
}

} // namespace Proxy
} // namespace Graphics
