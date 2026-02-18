#pragma once

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
}

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
    size_t mActiveParticlesCount;

    std::shared_ptr<ShaderStorageBufferObject> m_gpuParticlePositionsSSBO;

    std::shared_ptr<ParticleComputeShader_t> m_computeShader;

private:
    std::shared_ptr<ParticleShader_t> GetShader() const;

public:
    GpuParticleSystemSceneProxy(const ::EngineCore::GpuParticleSystemComponent* component);

    ~GpuParticleSystemSceneProxy() override;

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

    void SetActiveParticlesCount(const size_t activeParticlesCount);

    void SetParticlesPositionsData(const void* positionsData, const size_t byteChunkSize);

    RenderInfo GetRenderInfo() const override;
};
} // namespace Proxy
} // namespace Graphics
