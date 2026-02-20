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
    , mParticlesEmitted(false)
    , mPrevActiveParticlesCount(0)
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

    ShaderParams computeShaderParams("GpuParticleComputeShader");
    computeShaderParams.SetComputeShader(
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "compute" + SLASH + "gpuParticleCS.glsl");
    m_computeShader = ShaderPool::GetInstance()->GetOrAllocateResource<ParticleComputeShader_t>(computeShaderParams);

    mRenderData.mParticleMeshParams.mVertexAttributes = GetShader()->GetVertexAttributes();

    const uint32_t bytesToAllocate = static_cast<uint32_t>(mRenderData.mParticleMeshParams.mParticleCount * sizeof(glm::vec4));
    SSBOPoolParameters positionSSBOParams{bytesToAllocate, 0, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticlePositionsSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(positionSSBOParams);
    m_gpuParticlePositionsSSBO->SendDataToGPU();

    SSBOPoolParameters colorSSBOParams{bytesToAllocate, 1, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticleColorsSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(colorSSBOParams);
    m_gpuParticleColorsSSBO->SendDataToGPU();

    SSBOPoolParameters aliveCounterSSBOParams{sizeof(uint32_t), 2, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT};
    m_aliveCounterSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(aliveCounterSSBOParams);
    m_aliveCounterSSBO->SendDataToGPU();

    m_skin = ParticlesPool::GetInstance()->GetOrAllocateResource(mRenderData.mParticleMeshParams);
}

std::shared_ptr<typename GpuParticleSystemSceneProxy::ParticleShader_t> GpuParticleSystemSceneProxy::GetShader() const
{
    return std::static_pointer_cast<GpuParticleSystemSceneProxy::ParticleShader_t>(m_shader);
}

void GpuParticleSystemSceneProxy::CleanUp()
{
    PrimitiveSceneProxy::CleanUp();

    if (m_computeShader) {
        ShaderPool::GetInstance()->TryToFreeMemory(m_computeShader);
        m_computeShader = nullptr;
    }

    if (m_gpuParticlePositionsSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticlePositionsSSBO);
        m_gpuParticlePositionsSSBO = nullptr;
    }

    if (m_gpuParticleColorsSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticleColorsSSBO);
        m_gpuParticleColorsSSBO = nullptr;
    }

    if (m_aliveCounterSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_aliveCounterSSBO);
        m_aliveCounterSSBO = nullptr;
    }
}

void GpuParticleSystemSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    if (not mPrevActiveParticlesCount && not mParticlesEmitted) {
        return;
    }

    if (mParticlesEmitted) {
        mParticlesEmitted = false;
    }

    m_gpuParticlePositionsSSBO->BindBuffer();
    m_gpuParticleColorsSSBO->BindBuffer();
    m_aliveCounterSSBO->BindBuffer();

    uint32_t zero = 0;
    m_aliveCounterSSBO->BufferSubData(0, sizeof(uint32_t), &zero); // drop counter to zero before compute shader execution

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(m_computeShader->GetShaderName());
    if (needToRebindShader) {
        m_computeShader->ExecuteShader();
    }
    // update dispatch delta time
    {
        const auto currentTime = EngineTime::GetCurrentTime();
        const double timeSinceLastDispatch = EngineTime::GetTimeDifferenceInSeconds(currentTime - m_lastDispatchTime);
        m_lastDispatchTime = currentTime;
        m_computeShader->SetDispatchDeltaTime(timeSinceLastDispatch);
        m_computeShader->SetParticleLifetime(4.0f);
    }

    m_computeShader->Dispatch(mRenderData.mParticleMeshParams.mParticleCount, 1, 1);
    m_computeShader->SetMemoryBarrier(eMemoryBarrierType::ShaderStorageBarrierBit);

    GLsync fence = glFenceSync(
        GL_SYNC_GPU_COMMANDS_COMPLETE, 0); // ensure compute shader has finished execution before reading back alive counter
    glWaitSync(fence, 0, GL_TIMEOUT_IGNORED);
    glDeleteSync(fence);

    uint32_t* countOfAliveParticles = m_aliveCounterSSBO->GetMappedData<uint32_t>();
    ext_assert(countOfAliveParticles != nullptr, "Failed to map alive counter SSBO");
    mPrevActiveParticlesCount = *countOfAliveParticles;

    const auto& shader = GetShader();
    activeBindedState.TryUpdateActiveShaderName(m_shader->GetShaderName());
    m_shader->ExecuteShader();

    shader->GetVertexFactoryShader()->SetMatrices(m_worldMatrix, viewMatrix, projectionMatrix);
    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    m_skin->GetBuffer()->RenderInstanced(GL_POINTS, *countOfAliveParticles);
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

void GpuParticleSystemSceneProxy::ResetParticlesData(
    const void* positionsData, const size_t positionsDataSize, const void* colorsData, const size_t colorsDataSize)
{
    ext_assert(
        colorsData != nullptr && colorsDataSize > 0, "Data pointer is null or byte chunk size is zero in SetParticlesColorsData");
    ext_assert(
        positionsData != nullptr && positionsDataSize > 0,
        "Data pointer is null or byte chunk size is zero in SetParticlesPositionsData");
    ext_assert(
        m_gpuParticlePositionsSSBO,
        "SSBO is null in SetParticlesPositionsData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticlePositionsSSBO->GetAllocatedBufferSize() >= positionsDataSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesPositionsData");
    ext_assert(
        m_gpuParticleColorsSSBO->GetAllocatedBufferSize() >= colorsDataSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesColorsData");

    m_gpuParticlePositionsSSBO->BufferSubData(0, positionsDataSize, positionsData);
    m_gpuParticleColorsSSBO->BufferSubData(0, colorsDataSize, colorsData);

    mParticlesEmitted = true;
}

RenderInfo GpuParticleSystemSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Graphics::Proxy
