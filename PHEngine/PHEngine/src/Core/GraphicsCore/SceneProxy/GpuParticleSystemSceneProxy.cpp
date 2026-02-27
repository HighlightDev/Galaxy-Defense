#include "GpuParticleSystemSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ParticleComponents/GpuParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/IGpuParticleModuleProxy.h"
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
    , m_gpuParticleModulesProxies()
{
    m_gpuParticleModulesProxies = component->GetParticleModulesProxies();
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
    for (const auto& moduleProxy : m_gpuParticleModulesProxies) {
        computeShaderParams.AddShaderCodeSnippet(
            moduleProxy->GetModuleTypeHash(), eShaderType::ComputeShader, moduleProxy->GetShaderSnippet());
    }

    m_computeShader = ShaderPool::GetInstance()->GetOrAllocateResource<ParticleComputeShader_t>(computeShaderParams);

    mRenderData.mParticleMeshParams.mVertexAttributes = GetShader()->GetVertexAttributes();

    const uint32_t vec4BytesToAllocate
        = static_cast<uint32_t>(mRenderData.mParticleMeshParams.mParticleCount * sizeof(glm::vec4));
    const uint32_t vec2BytesToAllocate
        = static_cast<uint32_t>(mRenderData.mParticleMeshParams.mParticleCount * sizeof(glm::vec2));
    const uint32_t floatBytesToAllocate = static_cast<uint32_t>(mRenderData.mParticleMeshParams.mParticleCount * sizeof(float));
    SSBOPoolParameters positionSSBOParams{vec4BytesToAllocate, 0, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticlePositionsSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(positionSSBOParams);
    m_gpuParticlePositionsSSBO->SendDataToGPU();

    SSBOPoolParameters velocitySSBOParams{vec4BytesToAllocate, 1, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticleVelocitiesSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(velocitySSBOParams);
    m_gpuParticleVelocitiesSSBO->SendDataToGPU();

    SSBOPoolParameters initialVelocitySSBOParams{vec4BytesToAllocate, 2, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticleInitialVelocitiesSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(initialVelocitySSBOParams);
    m_gpuParticleInitialVelocitiesSSBO->SendDataToGPU();

    SSBOPoolParameters colorSSBOParams{vec4BytesToAllocate, 3, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticleColorsSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(colorSSBOParams);
    m_gpuParticleColorsSSBO->SendDataToGPU();

    SSBOPoolParameters rotationAndSizeSSBOParams{vec2BytesToAllocate, 4, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticleRotationAndSizeSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(rotationAndSizeSSBOParams);
    m_gpuParticleRotationAndSizeSSBO->SendDataToGPU();

    SSBOPoolParameters lifetimeSSBOParams{floatBytesToAllocate, 5, GL_DYNAMIC_STORAGE_BIT};
    m_gpuParticleLifetimeSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(lifetimeSSBOParams);
    m_gpuParticleLifetimeSSBO->SendDataToGPU();

    SSBOPoolParameters aliveCounterSSBOParams{sizeof(uint32_t), 6, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT};
    m_gpuParticlesAliveCounterSSBO = SSBOPool::GetInstance()->GetOrAllocateResource(aliveCounterSSBOParams);
    m_gpuParticlesAliveCounterSSBO->SendDataToGPU();

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

    if (m_gpuParticleVelocitiesSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticleVelocitiesSSBO);
        m_gpuParticleVelocitiesSSBO = nullptr;
    }

    if (m_gpuParticleInitialVelocitiesSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticleInitialVelocitiesSSBO);
        m_gpuParticleInitialVelocitiesSSBO = nullptr;
    }

    if (m_gpuParticleColorsSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticleColorsSSBO);
        m_gpuParticleColorsSSBO = nullptr;
    }

    if (m_gpuParticleRotationAndSizeSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticleRotationAndSizeSSBO);
        m_gpuParticleRotationAndSizeSSBO = nullptr;
    }

    if (m_gpuParticleLifetimeSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticleLifetimeSSBO);
        m_gpuParticleLifetimeSSBO = nullptr;
    }

    if (m_gpuParticlesAliveCounterSSBO) {
        SSBOPool::GetInstance()->TryToFreeMemory(m_gpuParticlesAliveCounterSSBO);
        m_gpuParticlesAliveCounterSSBO = nullptr;
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
    m_gpuParticleVelocitiesSSBO->BindBuffer();
    m_gpuParticleInitialVelocitiesSSBO->BindBuffer();
    m_gpuParticleRotationAndSizeSSBO->BindBuffer();
    m_gpuParticleLifetimeSSBO->BindBuffer();
    m_gpuParticlesAliveCounterSSBO->BindBuffer();

    uint32_t zero = 0;
    m_gpuParticlesAliveCounterSSBO->BufferSubData(
        0, sizeof(uint32_t), &zero); // drop counter to zero before compute shader execution

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
        m_computeShader->SetParticleMoveSpeed(15.0f);
    }

    m_computeShader->Dispatch(mRenderData.mParticleMeshParams.mParticleCount, 1, 1);
    m_computeShader->SetMemoryBarrier(eMemoryBarrierType::ShaderStorageBarrierBit);

    GLsync fence = glFenceSync(
        GL_SYNC_GPU_COMMANDS_COMPLETE, 0); // ensure compute shader has finished execution before reading back alive counter
    glWaitSync(fence, 0, GL_TIMEOUT_IGNORED);
    glDeleteSync(fence);

    uint32_t* countOfAliveParticles = m_gpuParticlesAliveCounterSSBO->GetMappedData<uint32_t>();
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
    const void* positionsData,
    const size_t positionsDataSize,
    const void* velocitiesData,
    const size_t velocitiesDataSize,
    const void* initialVelocitiesData,
    const size_t initialVelocitiesDataSize,
    const void* colorsData,
    const size_t colorsDataSize,
    const void* rotationAndSizeData,
    const size_t rotationAndSizeDataSize,
    const void* lifetimeData,
    const size_t lifetimeDataSize)
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
        m_gpuParticleVelocitiesSSBO,
        "SSBO is null in SetParticlesVelocitiesData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticleVelocitiesSSBO->GetAllocatedBufferSize() >= velocitiesDataSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesVelocitiesData");
    ext_assert(
        m_gpuParticleInitialVelocitiesSSBO,
        "SSBO is null in SetParticlesInitialVelocitiesData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticleInitialVelocitiesSSBO->GetAllocatedBufferSize() >= initialVelocitiesDataSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesInitialVelocitiesData");
    ext_assert(
        m_gpuParticleRotationAndSizeSSBO,
        "SSBO is null in SetParticlesRotationAndSizeData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticleRotationAndSizeSSBO->GetAllocatedBufferSize() >= rotationAndSizeDataSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesRotationAndSizeData");
    ext_assert(
        m_gpuParticleColorsSSBO, "SSBO is null in SetParticlesColorsData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticleColorsSSBO->GetAllocatedBufferSize() >= colorsDataSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesColorsData");
    ext_assert(
        m_gpuParticleLifetimeSSBO,
        "SSBO is null in SetParticlesLifetimeData, make sure PostConstructorInitialize was called before");
    ext_assert(
        m_gpuParticleLifetimeSSBO->GetAllocatedBufferSize() >= lifetimeDataSize,
        "Byte chunk size exceeds max allocated SSBO buffer size in SetParticlesLifetimeData");

    m_gpuParticlePositionsSSBO->BufferSubData(0, positionsDataSize, positionsData);
    m_gpuParticleVelocitiesSSBO->BufferSubData(0, velocitiesDataSize, velocitiesData);
    m_gpuParticleInitialVelocitiesSSBO->BufferSubData(0, initialVelocitiesDataSize, initialVelocitiesData);
    m_gpuParticleColorsSSBO->BufferSubData(0, colorsDataSize, colorsData);
    m_gpuParticleLifetimeSSBO->BufferSubData(0, lifetimeDataSize, lifetimeData);
    m_gpuParticleRotationAndSizeSSBO->BufferSubData(0, rotationAndSizeDataSize, rotationAndSizeData);

    mParticlesEmitted = true;
}

RenderInfo GpuParticleSystemSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}

void GpuParticleSystemSceneProxy::ResetParticleModulesProxies(
    const std::vector<std::shared_ptr<IGpuParticleModuleProxy>>& gpuParticleModulesProxies)
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_renderThreadName),
        "ResetParticleModulesProxies can be called only from RenderThread");
    ext_assert(
        m_computeShader,
        "Compute shader is null in ResetParticleModulesProxies, make sure PostConstructorInitialize was called before");

    ShaderPool::GetInstance()->TryToFreeMemory(m_computeShader);

    m_gpuParticleModulesProxies = gpuParticleModulesProxies;
    ShaderParams computeShaderParams("GpuParticleComputeShader");
    computeShaderParams.SetComputeShader(
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "compute" + SLASH + "gpuParticleCS.glsl");
    for (const auto& moduleProxy : m_gpuParticleModulesProxies) {
        computeShaderParams.AddShaderCodeSnippet(
            moduleProxy->GetModuleTypeHash(), eShaderType::ComputeShader, moduleProxy->GetShaderSnippet());
        m_computeShader = ShaderPool::GetInstance()->GetOrAllocateResource<ParticleComputeShader_t>(computeShaderParams);
    }
}
} // namespace Graphics::Proxy
