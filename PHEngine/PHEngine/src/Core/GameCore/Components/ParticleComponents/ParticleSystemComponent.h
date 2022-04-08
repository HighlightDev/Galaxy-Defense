#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"

namespace EngineCore
{
    struct ParticleModule;
    struct ParticleEmitter;
    struct ParticleSystemComponentData;
    struct ParticleSystemRenderData;

    class ParticleSystemComponent
        : public PrimitiveComponent
    {
        
        std::vector<ParticleModule*> mParticleModules;

        std::shared_ptr<ParticleEmitter*> mParticleEmitter;

    public:
        ParticleSystemComponent(const ParticleSystemComponentData& meshComponentData, const ParticleSystemRenderData& renderData);

        virtual ~ParticleSystemComponent();

        virtual void PostLevelInit() override;

        virtual ComponentType GetComponentType() const override;

        virtual void Tick(const float deltaTime) override;

        virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

        virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;
    };
}