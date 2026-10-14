#pragma once

#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Core/GameCore/Particles/ParticlesRawDataHandler.h"
#include "Core/GraphicsCore/RenderData/CpuParticleSystemRenderData.h"

using namespace Graphics::Data;

namespace EngineCore {
class IEmitter;

class CpuParticleSystemComponent : public ParticleSystemBaseComponent {
private:
    ParticlesRawDataHandler mParticlesRawDataHandler;

    CpuParticleSystemRenderData mRenderData;

public:
    CpuParticleSystemComponent(
        const std::shared_ptr<ParticleSystemComponentData>& meshComponentData, const CpuParticleSystemRenderData& renderData);

    ~CpuParticleSystemComponent() override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<Scripts::LuaProxy> ReplicateLuaProxy() override;

    void ResetParticles() override;

    ParticlesRawDataHandler& GetParticlesRawDataHandler();

    void UpdateWorldMatrix(const glm::mat4& parentWorldMatrix) override;

    inline const CpuParticleSystemRenderData& GetRenderData() const
    {
        return mRenderData;
    }

private:
    void SyncDataWithRenderThread(const size_t activeParticlesCount, const bool forceSyncData = false) override;
};
} // namespace EngineCore