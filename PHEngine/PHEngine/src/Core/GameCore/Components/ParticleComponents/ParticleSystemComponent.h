#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GraphicsCore/RenderData/ParticleSystemRenderData.h"
#include "Core/GameCore/Particles/Particle.h"
#include "Core/GameCore/Particles/ParticleProperties.h"
#include "Core/GameCore/Particles/ParticleProxyProperties.h"
#include "Core/GameCore/Particles/ParticleModule.h"

using namespace Graphics::Data;

namespace EngineCore
{
    struct ParticleEmitter;

    class ParticleSystemComponent
        : public PrimitiveComponent
    {

        std::vector<Particle> mParticlesPool;
        std::vector<ParticleProxyProperties> mParticleProxyPropertiesPool;

        std::vector<std::shared_ptr<ParticleModule>> mParticleModules;

        std::shared_ptr<ParticleEmitter *> mParticleEmitter;

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

        void EmitParticles(const std::vector<ParticleProperties> &particleProperties);

        inline const ParticleSystemRenderData& GetRenderData() const
        {
            return mRenderData;
        }
        
        void InitParticlePool();

    private:
        void SyncDataWithRenderThread();

    };
}