#pragma once

#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Particles/Modules/IParticleModule.h"
#include "Core/GameCore/Particles/Particle.h"
#include "Core/GameCore/Particles/ParticleProperties.h"
#include "Core/GameCore/Particles/ParticlesRawDataHandler.h"
#include "Core/GraphicsCore/RenderData/ParticleSystemRenderData.h"

using namespace Graphics::Data;

namespace EngineCore {
class IEmitter;

class ParticleSystemComponent : public PrimitiveComponent {
    friend class IEmitter;
    friend class IParticleModule;

    size_t mParticlesCount{0};

    std::vector<Particle> mParticlesPool;

    ParticlesRawDataHandler mParticlesRawDataHandler;

    size_t mPrevActiveParticles = 0;

    std::vector<std::shared_ptr<IParticleModule>> mParticleModules;

    std::shared_ptr<IEmitter> mParticleEmitter;

    ParticleSystemRenderData mRenderData;

public:
    ParticleSystemComponent(
        const std::shared_ptr<ParticleSystemComponentData>& meshComponentData, const ParticleSystemRenderData& renderData);

    ~ParticleSystemComponent() override;

    eComponentType GetComponentType() const override;

    void Tick(const float deltaTimeSec) override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    void AddParticleModule(const std::shared_ptr<IParticleModule>& particleModule);

    void EmitParticles();

    void ResetParticles();

    inline const ParticleSystemRenderData& GetRenderData() const
    {
        return mRenderData;
    }

    ParticlesRawDataHandler& GetParticlesRawDataHandler();

    size_t GetParticlesCount() const;

    void UpdateWorldMatrix(const glm::mat4& parentWorldMatrix) override;

    void SetParticleEmitter(const std::shared_ptr<IEmitter>& emitter);

private:
    void SyncDataWithRenderThread(const size_t activeParticlesCount, const bool forceSyncData = false);
};
} // namespace EngineCore