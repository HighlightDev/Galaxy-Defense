#include "BillboardSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/SimpleMeshType.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics {
namespace Proxy {

BillboardSceneProxy::BillboardSceneProxy(const BillboardComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , mRenderData(component->GetRenderData())
    , mBillboardExtent(component->GetBillboardExtent())
    , mViewMatrixTransformer(component->GetViewMatrixTransformer())
    , mProjectionMatrixTransformer(component->GetProjectionMatrixTransformer())
{
}

BillboardSceneProxy::~BillboardSceneProxy()
{
}

void BillboardSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("BillboardSceneProxy::PostConstructorInitialize");

    const ShaderParams shaderParams(
        "Billboard Shader",
        FolderManager::GetInstance()->GetShadersPath() + "billboardVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "billboardFS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "billboardGS.glsl");

    m_shader = CreateMaterialShader<StaticMeshVertexFactory, BillboardShader>(
        "StaticMeshVertexFactory_BillboardShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);
    SimplePrimitivePoolParameters poolParams;
    poolParams.mSimplePrimitiveType = SimplePrimitiveType::POINT;
    poolParams.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(poolParams);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
            const auto boundingBox = m_skin->GetBoundingBox();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mSceneProxyId, functionId, [this, sceneSp, boundingBox]() {
                    const auto& engineObject = sceneSp->GetEngineObjectById(GetGameObjectId());
                    assert(engineObject);
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    assert(primitiveComponent);
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

std::shared_ptr<typename BillboardSceneProxy::Shader_t> BillboardSceneProxy::GetShader() const
{
    return std::static_pointer_cast<typename BillboardSceneProxy::Shader_t>(m_shader);
}

void BillboardSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    const auto& billboardShader = GetShader();

    const auto& viewPortInfo = cameraSceneProxy->GetViewPort();
    const auto screenResolution = glm::vec2(viewPortInfo.Width, viewPortInfo.Height);
    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(billboardShader->GetShaderName());
    if (needToRebindShader)
    {
        billboardShader->ExecuteShader();
    }

    billboardShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    billboardShader->GetVertexFactoryShader()->SetMatrices(
        m_relativeMatrix, mViewMatrixTransformer(viewMatrix), mProjectionMatrixTransformer(projectionMatrix));
    billboardShader->GetShader()->SetExtent(mBillboardExtent);
    billboardShader->GetShader()->SetScreenResolution(screenResolution);
    m_skin->GetBuffer()->RenderVAO(GL_POINTS);
    //billboardShader->StopShader();
}

bool BillboardSceneProxy::IsDeferred() const
{
    return false;
}

bool BillboardSceneProxy::IsFrustumCullTestNeeded() const
{
    return false;
}

eMeshFacing BillboardSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

void BillboardSceneProxy::SetBillboardExtent(const float extent)
{
    mBillboardExtent = extent;
}

RenderInfo BillboardSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}

} // namespace Proxy
} // namespace Graphics
