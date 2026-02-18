#include "GpuParticleSystemSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ParticleComponents/GpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/GpuParticleVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/ShaderStorageBufferObject.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/ParticlesPool.h"
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

namespace Graphics::Proxy {
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

    CompositeShaderParams particlesCompositeShaderParams("GpuParticleVertexFactory_SimpleShader", particlesShaderParams);

    m_shader = CreateMaterialShader<GpuParticleVertexFactory, SimpleShader>(
        "GpuParticleVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, particlesShaderParams, mMaterialProxy);

    ShaderParams computeShaderParams("ComputeShader");
    computeShaderParams.SetComputeShader(
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "compute" + SLASH + "gpuParticleComputeShader.glsl");
    m_computeShader = ShaderPool::GetInstance()->GetOrAllocateResource<ParticleComputeShader_t>(computeShaderParams);

    mRenderData.mParticleMeshParams.mVertexAttributes = GetShader()->GetVertexAttributes();

    const uint32_t bytesToAllocate = static_cast<uint32_t>(mRenderData.mParticleMeshParams.mParticleCount * sizeof(glm::vec4));
    SSBOPoolParameters positionSSBOParams{bytesToAllocate, 0, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticlePositionsSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(positionSSBOParams);
    m_gpuParticlePositionsSSBO->SendDataToGPU();

    m_skin = ParticlesPool::GetInstance()->GetOrAllocateResource(mRenderData.mParticleMeshParams);
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

    m_gpuParticlePositionsSSBO->BindBuffer();
    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(m_computeShader->GetShaderName());
    if (needToRebindShader) {
        m_computeShader->ExecuteShader();
    }
    m_computeShader->Dispatch(mActiveParticlesCount, 1, 1);
    m_computeShader->SetMemoryBarrier(eMemoryBarrierType::ShaderStorageBarrierBit);

    const auto& shader = GetShader();
    activeBindedState.TryUpdateActiveShaderName(m_shader->GetShaderName());
    m_shader->ExecuteShader();

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

void GpuParticleSystemSceneProxy::SetParticlesPositionsData(const void* positionsData, const size_t byteChunkSize)
{
    ext_assert(
        positionsData != nullptr && byteChunkSize > 0,
        "Data pointer is null or byte chunk size is zero in SetParticlesPositionsData");
    ext_assert(
        m_gpuParticlePositionsSSBO,
        "SSBO is null in SetParticlesPositionsData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticlePositionsSSBO->GetAllocatedBufferSize() >= byteChunkSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesPositionsData");

    m_gpuParticlePositionsSSBO->BufferSubData(0, byteChunkSize, positionsData);
}

RenderInfo GpuParticleSystemSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Graphics::Proxy
