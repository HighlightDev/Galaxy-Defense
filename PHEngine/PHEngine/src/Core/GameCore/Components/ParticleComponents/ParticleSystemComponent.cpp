#include "ParticleSystemComponent.h"
#include "Core/GameCore/BoundingBox.h"

#include <glm/vec3.hpp>

namespace EngineCore
{
    ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponentData &meshComponentData, const ParticleSystemRenderData &renderData)
        : PrimitiveComponent("gameObjectName",
                             glm::vec3(),
                             glm::vec3(),
                             glm::vec3(1),
                             BoundingBox())
    {
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
    }

    void ParticleSystemComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    std::shared_ptr<PrimitiveSceneProxy> ParticleSystemComponent::CreateSceneProxy() const
    {
        return nullptr;
    }
}