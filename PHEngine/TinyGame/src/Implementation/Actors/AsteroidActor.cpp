#include "AsteroidActor.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/MovementComponent.h"

namespace Game
{
    AsteroidActor::AsteroidActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : SpaceObjectActor(gameObjectName, rootComponent)
    {
    }

    void AsteroidActor::TriggerSpawn(const glm::vec3 &position)
    {
        mActivityState = eSpaceObjectActivityState::ACTIVE;
        SetIsEnabled(true);
        GetMovementComponent()->Teleport(position);
    }

    void AsteroidActor::TriggerDisabled()
    {
        mActivityState = eSpaceObjectActivityState::IDLE;
        SetIsEnabled(false);
    }
}