#include "WaterPlaneSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/SimpleMeshType.h"

#include <glm/vec3.hpp>

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics {
namespace Proxy {
WaterPlaneSceneProxy::WaterPlaneSceneProxy(const WaterPlaneComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , m_waveStrength(component->GetWaveStrength())
    , m_transparencyDepth(component->GetTransparencyDepth())
    , m_nearClipPlane(component->GetNearClipPlane())
    , m_farClipPlane(component->GetFarClipPlane())
{
    Init();
}

void WaterPlaneSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("WaterPlaneSceneProxy::PostConstructorInitialize");

    const auto shaderIdName = "ForwardNonSkeletalBase Shader";
    const ShaderParams shaderParams(
        shaderIdName,
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "simpleVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "forwardFS.glsl");

    m_shader = CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
        "StaticMeshVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

    SimplePrimitivePoolParameters poolParams;
    poolParams.mSimplePrimitiveType = SimplePrimitiveType::PLANE_WITH_ATTRIBUTES;
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

std::shared_ptr<WaterPlaneSceneProxy::ShaderType> WaterPlaneSceneProxy::GetShader() const
{
    return std::static_pointer_cast<WaterPlaneSceneProxy::ShaderType>(m_shader);
}

WaterPlaneSceneProxy::~WaterPlaneSceneProxy()
{
}

void WaterPlaneSceneProxy::Init()
{
}

void WaterPlaneSceneProxy::SetWaveStrength(float waveStr)
{
    m_waveStrength = waveStr;
}

void WaterPlaneSceneProxy::SetTransparencyDepth(float transparencyDepth)
{
    m_transparencyDepth = transparencyDepth;
}

void WaterPlaneSceneProxy::SetNearClipPlane(float nearClipPlane)
{
    m_nearClipPlane = nearClipPlane;
}

void WaterPlaneSceneProxy::SetFarClipPlane(float farClipPlane)
{
    m_farClipPlane = farClipPlane;
}

ePrimitiveProxyType WaterPlaneSceneProxy::GetPrimitiveProxyType() const
{
    return ePrimitiveProxyType::PRIMITIVE_PROXY;
}

eMeshFacing WaterPlaneSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

bool WaterPlaneSceneProxy::IsDeferred() const
{
    return false;
}

void WaterPlaneSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    const auto& shader = GetShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shader->GetShaderName());
    if (needToRebindShader) {
        shader->ExecuteShader();
    }
    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
    m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
    //shader->StopShader();
}

RenderInfo WaterPlaneSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Proxy
} // namespace Graphics
