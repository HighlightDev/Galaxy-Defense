#pragma once

#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Core/GraphicsCore/RenderData/GpuParticleSystemRenderData.h"

using namespace Graphics::Data;

namespace EngineCore {
class IEmitter;

class GpuParticleSystemComponent : public ParticleSystemBaseComponent {

    GpuParticleSystemRenderData mRenderData;

    std::atomic_bool isParticlesDataDirty{false};

public:
    GpuParticleSystemComponent(
        const std::shared_ptr<ParticleSystemComponentData>& meshComponentData, const GpuParticleSystemRenderData& renderData);

    ~GpuParticleSystemComponent() override;

    void Tick(const float deltaTimeSec) override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<Scripts::LuaProxy> ReplicateLuaProxy() override;

    void UpdateWorldMatrix(const glm::mat4& parentWorldMatrix) override;

    void EmitParticles() override;

    inline const GpuParticleSystemRenderData& GetRenderData() const
    {
        return mRenderData;
    }

    void SetIsParticlesDataDirty(const bool isDirty);

    bool IsParticlesDataDirty() const;

private:
    void SyncDataWithRenderThread(const size_t activeParticlesCount, const bool forceSyncData = false) override;
};
} // namespace EngineCore