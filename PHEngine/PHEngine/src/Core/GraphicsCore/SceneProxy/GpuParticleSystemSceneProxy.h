#pragma once

#include "Core/CommonCore/TimeHelper.h"
#include "Core/GameCore/ShaderImplementation/GpuParticleComputeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/CpuParticleVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/RenderData/GpuParticleSystemRenderData.h"
#include "Core/IoCore/FolderManager.h"
#include "PrimitiveSceneProxy.h"

#include <vector>

using namespace IO;
using namespace Graphics::Data;
using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace EngineCore {
class GpuParticleSystemComponent;
class IGpuParticleModuleProxy;
} // namespace EngineCore

namespace Graphics::OpenGL {
class ShaderStorageBufferObject;
}

namespace Graphics {
namespace Proxy {
class GpuParticleSystemSceneProxy : public PrimitiveSceneProxy {
    using Base = PrimitiveSceneProxy;
    using ParticleShader_t = VertexFactoryMaterialCompositeShader<CpuParticleVertexFactory, SimpleShader>;
    using ParticleComputeShader_t = GpuParticleComputeShader;

    GpuParticleSystemRenderData mRenderData;

    bool mParticlesEmitted;

    uint32_t mPrevActiveParticlesCount;

    std::shared_ptr<ShaderStorageBufferObject> m_gpuParticlePositionsSSBO;
    std::shared_ptr<ShaderStorageBufferObject> m_gpuParticleVelocitiesSSBO;
    std::shared_ptr<ShaderStorageBufferObject> m_gpuParticleInitialVelocitiesSSBO;
    std::shared_ptr<ShaderStorageBufferObject> m_gpuParticleColorsSSBO;
    std::shared_ptr<ShaderStorageBufferObject> m_gpuParticleRotationAndSizeSSBO;
    std::shared_ptr<ShaderStorageBufferObject> m_aliveCounterSSBO;

    std::shared_ptr<ParticleComputeShader_t> m_computeShader;

    Moment_t m_lastDispatchTime;

    std::vector<std::shared_ptr<IGpuParticleModuleProxy>> m_gpuParticleModulesProxies;

private:
    std::shared_ptr<ParticleShader_t> GetShader() const;

public:
    GpuParticleSystemSceneProxy(const ::EngineCore::GpuParticleSystemComponent* component);

    void CleanUp() override;

    void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) override;

    void PostConstructorInitialize() override;

    bool IsDeferred() const override;

    eMeshFacing GetMeshFrontFace() const override;

    bool IsFrustumCullTestNeeded() const override;

    void ResetParticlesData(
        const void* positionsData,
        const size_t positionsDataSize,
        const void* velocitiesData,
        const size_t velocitiesDataSize,
        const void* initialVelocitiesData,
        const size_t initialVelocitiesDataSize,
        const void* colorsData,
        const size_t colorsDataSize,
        const void* rotationAndSizeData,
        const size_t rotationAndSizeDataSize);

    RenderInfo GetRenderInfo() const override;

    void ResetParticleModulesProxies(const std::vector<std::shared_ptr<IGpuParticleModuleProxy>>& gpuParticleModulesProxies);
};
} // namespace Proxy
} // namespace Graphics
