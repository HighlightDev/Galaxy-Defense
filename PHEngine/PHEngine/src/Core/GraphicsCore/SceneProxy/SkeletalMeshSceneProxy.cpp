#include "SkeletalMeshSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"

using namespace Graphics::Renderer;
using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace EngineCore;
using namespace Resources;

namespace Graphics {
namespace Proxy {
SkeletalMeshSceneProxy::SkeletalMeshSceneProxy(const SkeletalMeshComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , mRenderData(component->GetRenderData())
    , mAnimationPlayer(nullptr)
{
}

SkeletalMeshSceneProxy::~SkeletalMeshSceneProxy()
{
}

void SkeletalMeshSceneProxy::CleanUp()
{
    PrimitiveSceneProxy::CleanUp();

    mAnimationPlayer = nullptr;
}

void SkeletalMeshSceneProxy::PostConstructorInitialize()
{
    PrimitiveSceneProxy::PostConstructorInitialize();
    static constexpr uint64_t functionId = Hash64_CT("SkeletalMeshSceneProxy::PostConstructorInitialize");

    const ShaderParams shaderParams(
        "DeferredNonSkeletalBase Shader",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "simpleVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "deferredFS.glsl");

    m_shader = CreateMaterialShader<SkeletalMeshVertexFactory<4>, SimpleShader>(
        "SkeletalMeshVertexFactory<4>_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

    const ShaderParams planarReflectionParams(
        "PlanarReflectionShader",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "planarReflectionVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "forwardFS.glsl");

    m_planarReflectionShader = CreateMaterialShader<SkeletalMeshVertexFactory<4>, CapturePlanarReflectionShader>(
        "SkeletalMeshVertexFactory<4>_CapturePlanarReflectionShader" + mMaterialProxy->MaterialName,
        planarReflectionParams,
        mMaterialProxy);

    MeshPoolParameters poolParameters;
    poolParameters.mModelPath = mRenderData.mModelPath;
    poolParameters.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_skin = MeshPool::GetInstance()->GetOrAllocateResource(poolParameters);
    std::shared_ptr<AnimatedSkin> animatedSkinSp = std::dynamic_pointer_cast<AnimatedSkin>(m_skin);
    assert((animatedSkinSp));
    mAnimationPlayer = std::make_shared<AnimationPlayer>(animatedSkinSp);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
            const auto boundingBox = m_skin->GetBoundingBox();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                mSceneProxyId,
                functionId,
                [sceneSp, boundingBox, goID = GetGameObjectId()]() {
                    const auto& engineObject = sceneSp->GetEngineObjectById(goID);
                    assert(engineObject);
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    assert(primitiveComponent);
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

std::shared_ptr<SkeletalMeshSceneProxy::ShaderType> SkeletalMeshSceneProxy::GetShader() const
{
    return std::static_pointer_cast<SkeletalMeshSceneProxy::ShaderType>(m_shader);
}

std::shared_ptr<SkeletalMeshSceneProxy::PlanarReflectionShaderType> SkeletalMeshSceneProxy::GetPlanarReflectionShader() const
{
    return std::static_pointer_cast<SkeletalMeshSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
}

void SkeletalMeshSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    const auto& shader = GetShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shader->GetShaderName());
    if (needToRebindShader) {
        shader->ExecuteShader();
    }
    shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
    shader->GetVertexFactoryShader()->SetSkinningMatrices(GetSkinningMatrices());
    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
}

void SkeletalMeshSceneProxy::RenderPlanarReflection(
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
    planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrix, projectionMatrix);
    planarReflectionShader->GetVertexFactoryShader()->SetSkinningMatrices(GetSkinningMatrices());
    planarReflectionShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
}

void SkeletalMeshSceneProxy::UpdateAnimationData(
    bool transtionEnabled,
    const float transitionValue,
    const float srcAnimationTime,
    const float dstAnimationTime,
    const std::string& srcAnimationName,
    const std::string& dstAnimationName)
{
    mAnimationPlayer->SetSrcAnimationName(srcAnimationName);
    mAnimationPlayer->SetDstAnimationName(dstAnimationName);
    mAnimationPlayer->SetSrcAnimationTime(srcAnimationTime);
    mAnimationPlayer->SetDstAnimationTime(dstAnimationTime);
    mAnimationPlayer->SetTransitionParameter(transtionEnabled, transitionValue);

    bAnimationDataIsDirty = true;
}

void SkeletalMeshSceneProxy::UpdateAnimationData(
    bool transtionEnabled,
    const float transitionValue,
    const float srcAnimationTime,
    const float dstAnimationTime,
    const size_t srcAnimationIndex,
    const size_t dstAnimationIndex)
{
    mAnimationPlayer->SetSrcAnimationTime(srcAnimationTime);
    mAnimationPlayer->SetDstAnimationTime(dstAnimationTime);
    mAnimationPlayer->SetSrcAnimationByIndex(srcAnimationIndex);
    mAnimationPlayer->SetDstAnimationByIndex(dstAnimationIndex);
    mAnimationPlayer->SetTransitionParameter(transtionEnabled, transitionValue);

    bAnimationDataIsDirty = true;
}

ePrimitiveProxyType SkeletalMeshSceneProxy::GetPrimitiveProxyType() const
{
    return ePrimitiveProxyType::SKELETAL_MESH_PROXY;
}

const std::vector<glm::mat4>& SkeletalMeshSceneProxy::GetSkinningMatrices() const
{
    if (bAnimationDataIsDirty) {
        mAnimationPlayer->UpdateAnimationMatrices();
        bAnimationDataIsDirty = false;
    }

    return mAnimationPlayer->GetAnimatedMatrices();
}

bool SkeletalMeshSceneProxy::IsDeferred() const
{
    return true;
}

eMeshFacing SkeletalMeshSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

RenderInfo SkeletalMeshSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Proxy
} // namespace Graphics
