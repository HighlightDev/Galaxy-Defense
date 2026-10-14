#include "CpuParticleSystemSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/ParticlesPool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineMath.h"

#include <stdlib.h>

using namespace EngineMath;
using namespace Resources;
using namespace TinyLogger;
using namespace Graphics::Renderer;
using namespace Graphics::OpenGL;
using namespace EngineCore;

namespace Graphics {
namespace Proxy {
CpuParticleSystemSceneProxy::CpuParticleSystemSceneProxy(const CpuParticleSystemComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , mParticlesRawDataHandler(component->GetParticlesCount())
    , mRenderData(component->GetRenderData())
    , mActiveParticlesCount(0)
{
}

void CpuParticleSystemSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("CpuParticleSystemSceneProxy::PostConstructorInitialize");

    ShaderParams particlesShaderParams("ParticleShader");
    particlesShaderParams.SetMainShaders(
        FolderManager::GetInstance()->GetAbsolutePathToRes("particleVS.glsl"),
        FolderManager::GetInstance()->GetAbsolutePathToRes("particleFS.glsl"));
    particlesShaderParams.SetGeometryShader(FolderManager::GetInstance()->GetAbsolutePathToRes("particleGS.glsl"));

    CompositeShaderParams particlesCompositeShaderParams("ParticleVertexFactory_SimpleShader", particlesShaderParams);

    m_shader = CreateMaterialShader<CpuParticleVertexFactory, SimpleShader>(
        "ParticleVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, particlesShaderParams, mMaterialProxy);

    mRenderData.mParticleMeshParams.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_skin = ParticlesPool::GetInstance()->GetOrAllocateResource(mRenderData.mParticleMeshParams);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
            const auto boundingBox = m_skin->GetBoundingBox();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                mSceneProxyId,
                functionId,
                [boundingBox, sceneSp, goID = GetGameObjectId()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    const auto& engineObject = sceneSp->GetEngineObjectById(goID);
                    ext_assert(engineObject, "Engine object not found by ID in CpuParticleSystemSceneProxy");
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    ext_assert(
                        primitiveComponent, "Failed to cast engine object to PrimitiveComponent in CpuParticleSystemSceneProxy");
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

CpuParticleSystemSceneProxy::~CpuParticleSystemSceneProxy()
{
}

std::shared_ptr<typename CpuParticleSystemSceneProxy::ParticleShader_t> CpuParticleSystemSceneProxy::GetShader() const
{
    return std::static_pointer_cast<CpuParticleSystemSceneProxy::ParticleShader_t>(m_shader);
}

void CpuParticleSystemSceneProxy::CleanUp()
{
    PrimitiveSceneProxy::CleanUp();
}

void CpuParticleSystemSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    if (!mActiveParticlesCount)
        return;

    const auto& shader = GetShader();

    PrepareParticlesInstancedBuffer();
    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shader->GetShaderName());
    if (needToRebindShader) {
        shader->ExecuteShader();
    }
    shader->GetVertexFactoryShader()->SetMatrices(m_worldMatrix, viewMatrix, projectionMatrix);
    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    m_skin->GetBuffer()->RenderInstanced(GL_POINTS, mActiveParticlesCount);
}

bool CpuParticleSystemSceneProxy::IsDeferred() const
{
    return false;
}

eMeshFacing CpuParticleSystemSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

bool CpuParticleSystemSceneProxy::IsFrustumCullTestNeeded() const
{
    return false;
}

void CpuParticleSystemSceneProxy::SetActiveParticlesCount(const size_t activeParticlesCount)
{
    mActiveParticlesCount = activeParticlesCount;
}

void CpuParticleSystemSceneProxy::CopyParticlesRawData(
    const void* translationBuffer,
    const size_t translationByteChunkSize,
    const void* rotationSizeBuffer,
    const size_t rotationByteChunkSize,
    const void* colorBuffer,
    const size_t colorByteChunkSize)
{
    if (translationByteChunkSize > 0) {
        mParticlesRawDataHandler.CopyToMeActiveTranslationData(translationBuffer, 0, translationByteChunkSize);
        mParticlesRawDataHandler.SetTranslationActiveDataChunkSize(translationByteChunkSize);
    }

    if (rotationByteChunkSize > 0) {
        mParticlesRawDataHandler.CopyToMeActiveRotationSizeData(rotationSizeBuffer, 0, rotationByteChunkSize);
        mParticlesRawDataHandler.SetRotationSizeActiveDataChunkSize(rotationByteChunkSize);
    }

    if (colorByteChunkSize > 0) {
        mParticlesRawDataHandler.CopyToMeActiveColorData(colorBuffer, 0, colorByteChunkSize);
        mParticlesRawDataHandler.SetColorActiveDataChunkSize(colorByteChunkSize);
    }

    bIsParticlesTransformDirty = true;
}

void CpuParticleSystemSceneProxy::PrepareParticlesInstancedBuffer()
{
    if (!bIsParticlesTransformDirty)
        return;
    auto* const particlesTransformVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleRelativeOffset");
    auto* const particlesRotationSizeVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleRotationAndSize");
    auto* const particlesColorVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleColor");

    ext_assert(particlesTransformVBO && particlesRotationSizeVBO && particlesColorVBO, "Failed to get particle system VBOs");

    const size_t translationSubBufferSize = mParticlesRawDataHandler.GetTranslationActiveDataChunkSize();
    const size_t rotationSizeSubBufferSize = mParticlesRawDataHandler.GetRotationSizeActiveDataChunkSize();
    const size_t colorBufferSize = mParticlesRawDataHandler.GetColorActiveDataChunkSize();
    particlesTransformVBO->BufferSubData(0, translationSubBufferSize, mParticlesRawDataHandler.GetTranslationData());
    particlesRotationSizeVBO->BufferSubData(0, rotationSizeSubBufferSize, mParticlesRawDataHandler.GetRotationSizeData());
    particlesColorVBO->BufferSubData(0, colorBufferSize, mParticlesRawDataHandler.GetColorData());
    particlesTransformVBO->UnbindBuffer();

    bIsParticlesTransformDirty = false;
}

RenderInfo CpuParticleSystemSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Proxy
} // namespace Graphics
