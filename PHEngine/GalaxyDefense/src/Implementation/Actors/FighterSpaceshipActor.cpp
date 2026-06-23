#include "FighterSpaceshipActor.h"

#include "Implementation/Actors/RouteHandlers/FighterSpaceshipRouteHandler.h"

#include <cmath>

namespace Game {
FighterSpaceshipActor::FighterSpaceshipActor(
    const std::string& gameObjectName,
    const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
    const FighterSpaceshipLevel& fighterLevel)
    : WeakSpaceshipActor(gameObjectName, rootComponent, fighterLevel)
    , mFighterLevel(fighterLevel)
{
    mSpaceshipType = eSpaceshipType::FIGHTER;
    mRouteHandler = std::make_unique<FighterSpaceshipRouteHandler>(this);
}

void FighterSpaceshipActor::Tick(const float deltaTimeSec, const float playSpeed)
{
    WeakSpaceshipActor::Tick(deltaTimeSec, playSpeed);

    mShootCooldown += deltaTimeSec * playSpeed;
}

void FighterSpaceshipActor::TriggerSpawn(const glm::vec3& position)
{
    WeakSpaceshipActor::TriggerSpawn(position);

    mShootCooldown = 0.0f;
    mIsRayActive = false;
}

void FighterSpaceshipActor::TriggerDisabled()
{
    WeakSpaceshipActor::TriggerDisabled();

    mShootCooldown = 0.0f;
    mIsRayActive = false;
}

bool FighterSpaceshipActor::CanShoot() const
{
    return !mIsRayActive && std::floor(mShootCooldown * 1000.0f) >= mFighterLevel.GetCooldownMs();
}

void FighterSpaceshipActor::RestartTimerSinceLastShoot()
{
    mShootCooldown = 0.0f;
}

void FighterSpaceshipActor::SetIsRayActive(const bool value)
{
    mIsRayActive = value;
}

bool FighterSpaceshipActor::GetIsRayActive() const
{
    return mIsRayActive;
}

const FighterSpaceshipLevel& FighterSpaceshipActor::GetFighterLevel() const
{
    return mFighterLevel;
}
} // namespace Game
