#include "SpaceStationActor.h"

#include "Core/GameCore/Components/SceneComponent.h"

namespace Game {
SpaceStationActor::SpaceStationActor(
    const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
{
}

void SpaceStationActor::Tick(const float deltaTime)
{
    Actor::Tick(deltaTime);

    mTimeSinceLastShoot += deltaTime;
}

bool SpaceStationActor::CanShoot() const
{
    constexpr float c_shootTimeout = 1.0f;
    return mTimeSinceLastShoot >= c_shootTimeout;
}

void SpaceStationActor::RestartTimerSinceLastShoot()
{
    mTimeSinceLastShoot = 0.0f;
}

void SpaceStationActor::SetState(const eSpaceStationActivityState spacestationState)
{
    if (mSpacestationState != spacestationState) {
        mSpacestationState = spacestationState;
        SetIsEnabled(!(spacestationState == eSpaceStationActivityState::IDLE));
    }
}

eSpaceStationActivityState SpaceStationActor::GetState() const
{
    return mSpacestationState;
}
} // namespace Game
