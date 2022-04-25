#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GraphicsCore/RenderData/ParticleSystemRenderData.h"
#include "Core/GameCore/Particles/Particle.h"
#include "Core/GameCore/Particles/ParticleProperties.h"
#include "Core/GameCore/Particles/ParticleModule.h"
#include "Core/GameCore/Particles/ParticlesRawDataHandler.h"
#include "Core/GameCore/Particles/Emitters/IEmitter.h"

using namespace Graphics::Data;

namespace EngineCore
{
    struct ParticleEmitter;

    class ParticleSystemComponent
        : public PrimitiveComponent
    {
        friend class IEmitter;

        std::vector<Particle> mParticlesPool;

        ParticlesRawDataHandler mParticlesRawDataHandler;

        size_t mPrevActiveParticles = 0;

        std::vector<std::shared_ptr<ParticleModule>> mParticleModules;

        std::shared_ptr<IEmitter> mParticleEmitter;

        ParticleSystemRenderData mRenderData;

    public:
        ParticleSystemComponent(const ParticleSystemComponentData &meshComponentData,
                                const ParticleSystemRenderData &renderData);

        virtual ~ParticleSystemComponent();

        virtual void PostLevelInit() override;

        virtual ComponentType GetComponentType() const override;

        virtual void Tick(const float deltaTime) override;

        virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

        virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

        void AddParticleModule(const std::shared_ptr<ParticleModule> &particleModule);

        void EmitParticles(const size_t particlesCount = 0);

        inline const ParticleSystemRenderData &GetRenderData() const
        {
            return mRenderData;
        }

        size_t GetParticlesCount() const;

        virtual void UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix) override;
        
        void SetParticleEmitter(const std::shared_ptr<IEmitter>& emitter);

    private:
        void SyncDataWithRenderThread(const size_t activeParticlesCount);
    };
}