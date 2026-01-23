#include "GpuParticleSystemSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ParticleComponents/GpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/OpenGL/ShaderStorageBufferObject.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/ParticlesPool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"
#include "Core/ResourceManagerCore/Pool/SSBOPool.h"
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
GpuParticleSystemSceneProxy::GpuParticleSystemSceneProxy(const GpuParticleSystemComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , mRenderData(component->GetRenderData())
    , mActiveParticlesCount(0)
{
}

void GpuParticleSystemSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("GpuParticleSystemSceneProxy::PostConstructorInitialize");

    ShaderParams particlesShaderParams("ParticleShader");
    particlesShaderParams.SetMainShaders(
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleFS.glsl");
    particlesShaderParams.SetGeometryShader(FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleGS.glsl");

    CompositeShaderParams particlesCompositeShaderParams("ParticleVertexFactory_SimpleShader", particlesShaderParams);

    m_shader = CreateMaterialShader<ParticleVertexFactory, SimpleShader>(
        "ParticleVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, particlesShaderParams, mMaterialProxy);

    mRenderData.mParticleMeshParams.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_gpuParticlesSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(mRenderData.mSSBOPoolParams);

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
                    ext_assert(engineObject, "Engine object not found by ID in GpuParticleSystemSceneProxy");
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    ext_assert(
                        primitiveComponent, "Failed to cast engine object to PrimitiveComponent in GpuParticleSystemSceneProxy");
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

GpuParticleSystemSceneProxy::~GpuParticleSystemSceneProxy()
{
}

std::shared_ptr<typename GpuParticleSystemSceneProxy::ParticleShader_t> GpuParticleSystemSceneProxy::GetShader() const
{
    return std::static_pointer_cast<GpuParticleSystemSceneProxy::ParticleShader_t>(m_shader);
}

void GpuParticleSystemSceneProxy::CleanUp()
{
    PrimitiveSceneProxy::CleanUp();
}

void GpuParticleSystemSceneProxy::Render(
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

bool GpuParticleSystemSceneProxy::IsDeferred() const
{
    return false;
}

eMeshFacing GpuParticleSystemSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

bool GpuParticleSystemSceneProxy::IsFrustumCullTestNeeded() const
{
    return false;
}

void GpuParticleSystemSceneProxy::SetActiveParticlesCount(const size_t activeParticlesCount)
{
    mActiveParticlesCount = activeParticlesCount;
}

void GpuParticleSystemSceneProxy::PrepareParticlesInstancedBuffer()
{
    if (!bIsParticlesTransformDirty)
        return;
    auto* const particlesTransformVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleRelativeOffset");
    auto* const particlesRotationSizeVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleRotationAndSize");
    auto* const particlesColorVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleColor");

    ext_assert(particlesTransformVBO && particlesRotationSizeVBO && particlesColorVBO, "Failed to get particle system VBOs");

    // const size_t translationSubBufferSize = mParticlesRawDataHandler.GetTranslationActiveDataChunkSize();
    // const size_t rotationSizeSubBufferSize = mParticlesRawDataHandler.GetRotationSizeActiveDataChunkSize();
    // const size_t colorBufferSize = mParticlesRawDataHandler.GetColorActiveDataChunkSize();
    // particlesTransformVBO->BufferSubData(0, translationSubBufferSize, mParticlesRawDataHandler.GetTranslationData());
    // particlesRotationSizeVBO->BufferSubData(0, rotationSizeSubBufferSize, mParticlesRawDataHandler.GetRotationSizeData());
    // particlesColorVBO->BufferSubData(0, colorBufferSize, mParticlesRawDataHandler.GetColorData());
    particlesTransformVBO->UnbindBuffer();

    bIsParticlesTransformDirty = false;
}

void GpuParticleSystemSceneProxy::SetParticlesPositionsData(const void* positionsData, const size_t byteChunkSize)
{
    ext_assert(
        positionsData != nullptr && byteChunkSize > 0,
        "Data pointer is null or byte chunk size is zero in SetParticlesPositionsData");
    ext_assert(
        m_gpuParticlesSSBO, "SSBO is null in SetParticlesPositionsData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticlesSSBO->GetAllocatedBufferSize() >= byteChunkSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesPositionsData");

    m_gpuParticlesSSBO->BufferSubData(0, byteChunkSize, positionsData);
}

RenderInfo GpuParticleSystemSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Proxy
} // namespace Graphics
