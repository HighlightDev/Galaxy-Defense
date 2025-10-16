#include "StaticMeshSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics {
namespace Proxy {
StaticMeshSceneProxy::StaticMeshSceneProxy(const StaticMeshComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , m_renderData(component->GetRenderData())
{
}

StaticMeshSceneProxy::~StaticMeshSceneProxy()
{
}

void StaticMeshSceneProxy::PostConstructorInitialize()
{
    PrimitiveSceneProxy::PostConstructorInitialize();
    static constexpr uint64_t functionId = Hash64_CT("StaticMeshSceneProxy::PostConstructorInitialize");

    const auto shaderIdName = m_renderData.mIsDeferredShaded ? "DeferredNonSkeletalBase Shader" : "ForwardNonSkeletalBase Shader";
    const auto fragmentShaderName = m_renderData.mIsDeferredShaded ? "deferredFS.glsl" : "forwardFS.glsl";
    const ShaderParams shaderParams(
        shaderIdName,
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "simpleVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + fragmentShaderName);

    m_shader = CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
        "StaticMeshVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

    const ShaderParams planarReflectionParams(
        "PlanarReflectionShader",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "planarReflectionVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "forwardFS.glsl");

    m_planarReflectionShader = CreateMaterialShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>(
        "StaticMeshVertexFactory_CapturePlanarReflectionShader_" + mMaterialProxy->MaterialName,
        planarReflectionParams,
        mMaterialProxy);

    MeshPoolParameters poolParameters;
    poolParameters.mModelPath = m_renderData.mModelPath;
    poolParameters.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_skin = MeshPool::GetInstance()->GetOrAllocateResource(poolParameters);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
            if (const auto& outlineMatProxySp = sceneSp->GetOutlineMaterial()->GetMaterialProxyWp().lock()) {
                mOutlineMaterialProxy = outlineMatProxySp;
                const ShaderParams outlineShaderParams(
                    "OutlineShader",
                    FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "simpleVS.glsl",
                    FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + fragmentShaderName);
                m_outlineShader = CreateMaterialShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>(
                    "StaticMeshVertexFactory_OutlineShader_OutlineMaterial", outlineShaderParams, outlineMatProxySp);
            }

            const auto boundingBox = m_skin->GetBoundingBox();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                mSceneProxyId,
                functionId,
                [sceneSp, boundingBox, goID = GetGameObjectId()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    const auto& engineObject = sceneSp->GetEngineObjectById(goID);
                    assert(engineObject);
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    assert(primitiveComponent);
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

std::shared_ptr<StaticMeshSceneProxy::ShaderType> StaticMeshSceneProxy::GetShader() const
{
    return std::static_pointer_cast<StaticMeshSceneProxy::ShaderType>(m_shader);
}

std::shared_ptr<StaticMeshSceneProxy::PlanarReflectionShaderType> StaticMeshSceneProxy::GetPlanarReflectionShader() const
{
    return std::static_pointer_cast<StaticMeshSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
}

void StaticMeshSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    const auto& mainShader = GetShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(mainShader->GetShaderName());
    if (needToRebindShader) {
        mainShader->ExecuteShader();
    }
    mainShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    mainShader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
    m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
}

void StaticMeshSceneProxy::RenderPlanarReflection(
    const glm::vec4& plane,
    const glm::mat4& mirrorMatrix,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    const auto& planarReflectionShader = GetPlanarReflectionShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(planarReflectionShader->GetShaderName());
    if (needToRebindShader) {
        planarReflectionShader->ExecuteShader();
    }
    planarReflectionShader->GetShader()->SetClipPlane(plane);
    planarReflectionShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrix, projectionMatrix);
    m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
}

void StaticMeshSceneProxy::RenderOutlineStencil(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    if (!m_outlineShader) {
        return;
    }
    const auto& outlineShader = std::static_pointer_cast<StaticMeshSceneProxy::OutlineShaderType>(m_outlineShader);

    if (mIsOutlineApplied) {
        const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(outlineShader->GetShaderName());
        if (needToRebindShader) {
            outlineShader->ExecuteShader();
        }
        outlineShader->GetMaterialShader()->LoadUniformValues(mOutlineMaterialProxy, activeBindedState);
        outlineShader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
        m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
    }
}

void StaticMeshSceneProxy::RenderOutline(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    if (!m_outlineShader) {
        return;
    }
    const auto& outlineShader = std::static_pointer_cast<StaticMeshSceneProxy::OutlineShaderType>(m_outlineShader);

    if (mIsOutlineApplied) {
        const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(outlineShader->GetShaderName());
        if (needToRebindShader) {
            outlineShader->ExecuteShader();
        }
        outlineShader->GetMaterialShader()->LoadUniformValues(mOutlineMaterialProxy, activeBindedState);
        outlineShader->GetVertexFactoryShader()->SetMatrices(m_outlineMatrix, viewMatrix, projectionMatrix);
        m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
    }
}

bool StaticMeshSceneProxy::IsDeferred() const
{
    return m_renderData.mIsDeferredShaded;
}

eMeshFacing StaticMeshSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

ePrimitiveProxyType StaticMeshSceneProxy::GetPrimitiveProxyType() const
{
    return ePrimitiveProxyType::STATIC_MESH_PROXY;
}

RenderInfo StaticMeshSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}

void StaticMeshSceneProxy::SetMeshModelPath(const std::string& modelPath)
{
    m_renderData.mModelPath = modelPath;
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));

    MeshPoolParameters poolParameters;
    poolParameters.mModelPath = modelPath;
    poolParameters.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_skin = MeshPool::GetInstance()->GetOrAllocateResource(poolParameters);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {

            const auto boundingBox = m_skin->GetBoundingBox();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                mSceneProxyId,
                Hash64_CT("SkeletalMeshSceneProxy::SetMeshModelPath"),
                [sceneSp, boundingBox, goID = GetGameObjectId()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    const auto& engineObject = sceneSp->GetEngineObjectById(goID);
                    assert(engineObject);
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    assert(primitiveComponent);
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

} // namespace Proxy
} // namespace Graphics
