#include "SkyboxSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/SimpleMeshType.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics {
namespace Proxy {
SkyboxSceneProxy::SkyboxSceneProxy(const SkyboxComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
{
}

void SkyboxSceneProxy::PostConstructorInitialize()
{
    PrimitiveSceneProxy::PostConstructorInitialize();
    static constexpr uint64_t functionId = Hash64_CT("SkyboxSceneProxy::PostConstructorInitialize");

    ShaderParams shaderParams = ShaderParams("SkyboxForwardShader");

    shaderParams.SetMainShaders(
        FolderManager::GetInstance()->GetAbsolutePathToRes("simpleVS.glsl"),
        FolderManager::GetInstance()->GetAbsolutePathToRes("forwardNoLitFS.glsl"));

    m_shader = CreateMaterialShader<SkyboxVertexFactory, SimpleShader>(
        "SkyboxVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

    ShaderParams planarReflectionParams("PlanarReflectionShader");
    planarReflectionParams.SetMainShaders(
        FolderManager::GetInstance()->GetAbsolutePathToRes("planarReflectionVS.glsl"),
        FolderManager::GetInstance()->GetAbsolutePathToRes("forwardNoLitFS.glsl"));

    m_planarReflectionShader = CreateMaterialShader<SkyboxVertexFactory, CapturePlanarReflectionShader>(
        "SkyboxVertexFactory_CapturePlanarReflectionShader_" + mMaterialProxy->MaterialName,
        planarReflectionParams,
        mMaterialProxy);

    SimplePrimitivePoolParameters poolParams;
    poolParams.mSimplePrimitiveType = SimplePrimitiveType::INVERTED_VERTICES_DIRECTION_CUBE;
    poolParams.mVertexAttributes = GetShader()->GetVertexAttributes();
    m_skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(poolParams);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
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
                    ext_assert(engineObject, "Engine object not found by ID in SkyboxSceneProxy");
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    ext_assert(primitiveComponent, "Failed to cast engine object to PrimitiveComponent in SkyboxSceneProxy");
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

std::shared_ptr<SkyboxSceneProxy::ShaderType> SkyboxSceneProxy::GetShader() const
{
    return std::static_pointer_cast<SkyboxSceneProxy::ShaderType>(m_shader);
}

std::shared_ptr<SkyboxSceneProxy::PlanarReflectionShaderType> SkyboxSceneProxy::GetPlanarReflectionShader() const
{
    return std::static_pointer_cast<SkyboxSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
}

ePrimitiveProxyType SkyboxSceneProxy::GetPrimitiveProxyType() const
{
    return ePrimitiveProxyType::PRIMITIVE_PROXY;
}

void SkyboxSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    glm::mat4 viewMatrixNoTranslation = viewMatrix;
    viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

    auto shaderPtr = GetShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shaderPtr->GetShaderName());
    if (needToRebindShader) {
        shaderPtr->ExecuteShader();
    }
    shaderPtr->GetVertexFactoryShader()->SetMatrices(m_worldMatrix, viewMatrixNoTranslation, projectionMatrix);
    shaderPtr->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
}

void SkyboxSceneProxy::RenderPlanarReflection(
    const glm::vec4& plane,
    const glm::mat4& mirrorMatrix,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    const auto& planarReflectionShader = GetPlanarReflectionShader();

    glm::mat4 viewMatrixNoTranslation = viewMatrix;
    viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(planarReflectionShader->GetShaderName());
    if (needToRebindShader) {
        planarReflectionShader->ExecuteShader();
    }
    planarReflectionShader->GetShader()->SetClipPlane(plane);
    planarReflectionShader->GetVertexFactoryShader()->SetMatrices(
        mirrorMatrix * m_worldMatrix, viewMatrixNoTranslation, projectionMatrix);
    planarReflectionShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
}

bool SkyboxSceneProxy::IsFrustumCullTestNeeded() const
{
    return false;
}

bool SkyboxSceneProxy::IsDeferred() const
{
    return false;
}

eMeshFacing SkyboxSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

RenderInfo SkyboxSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Proxy
} // namespace Graphics