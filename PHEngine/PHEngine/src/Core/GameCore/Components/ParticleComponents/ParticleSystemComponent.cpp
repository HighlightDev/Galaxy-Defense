#include "ParticleSystemComponent.h"
#include "Core/GameCore/BoundingBox.h"

#include <glm/vec3.hpp>

namespace EngineCore
{
    ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponentData &meshComponentData
                                                    /* const ParticleSystemRenderData &renderData*/)
        : PrimitiveComponent(meshComponentData.GameObjectName,
                             meshComponentData.m_translation,
                             glm::vec3(),
                             glm::vec3(1.0f),
                             BoundingBox()),
          mParticlesPool()
    {
        mParticlesPool.resize(100);
    }

    ParticleSystemComponent::~ParticleSystemComponent()
    {
    }

    void ParticleSystemComponent::PostLevelInit()
    {
    }

    ComponentType ParticleSystemComponent::GetComponentType() const
    {
        return PRIMITIVE_COMPONENT;
    }

    void ParticleSystemComponent::Tick(const float deltaTime)
    {
        for (const auto& module : mParticleModules)
        {
            module->Tick(deltaTime);
        }
    }

    void ParticleSystemComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    std::shared_ptr<PrimitiveSceneProxy> ParticleSystemComponent::CreateSceneProxy() const
    {
        return nullptr;
    }

    void ParticleSystemComponent::AddParticleModule(const std::shared_ptr<ParticleModule> &particleModule)
    {
        mParticleModules.emplace_back(particleModule);
    }

    void ParticleSystemComponent::EmitParticles(const std::vector<ParticleProperties> &particleProperties)
    {
    }

    void ParticleSystemComponent::InitParticlePool()
    {
    }
}