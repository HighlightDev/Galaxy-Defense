#include "SpaceStationActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"

namespace Game {
SpaceStationActor::SpaceStationActor(
    const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent)
    : Actor(gameObjectName, rootComponent)
    , mSpaceStationLevel()
    , mShootRadiusProperty(std::make_shared<EngineObjectProperty<float>>(0.0f, "p_shootRadius"))
{
    AddEngineProperty(mShootRadiusProperty);
}

void SpaceStationActor::Tick(const float deltaTimeSec)
{
    Actor::Tick(deltaTimeSec);

    mShootCooldown += deltaTimeSec;
}

bool SpaceStationActor::CanShoot() const
{
    return !mIsRayActive && mSpaceStationLevel != nullptr
        && std::floor(mShootCooldown * 1000.0f) >= mSpaceStationLevel->GetCooldownMs();
}

void SpaceStationActor::SetIsRayActive(const bool value)
{
    mIsRayActive = value;
}

bool SpaceStationActor::GetIsRayActive() const
{
    return mIsRayActive;
}

void SpaceStationActor::RestartTimerSinceLastShoot()
{
    mShootCooldown = 0.0f;
}

void SpaceStationActor::SetState(const eSpaceStationActivityState spacestationState)
{
    if (mSpacestationState != spacestationState) {
        mSpacestationState = spacestationState;
        if (mSpacestationState == eSpaceStationActivityState::IDLE) {
            mSpaceStationLevel = nullptr;
        }
        // Disable spacestation if it is idle
        SetIsEnabled(spacestationState == eSpaceStationActivityState::ACTIVE);
    }
}

eSpaceStationActivityState SpaceStationActor::GetState() const
{
    return mSpacestationState;
}

void SpaceStationActor::SetSpaceStationLevel(const std::shared_ptr<SpaceStationLevel>& spaceStationLevel)
{
    assert(spaceStationLevel);
    mSpaceStationLevel = spaceStationLevel;
    const float shootRadius = mSpaceStationLevel->GetShootRadius();
    mShootRadiusProperty->SetValue(shootRadius);
    mRadiusMarkerComponent->SetScale(glm::vec3(shootRadius * 2.0f, 1.0f, shootRadius * 2.0f));
}

const std::shared_ptr<SpaceStationLevel>& SpaceStationActor::GetSpaceStationLevel() const
{
    assert(mSpaceStationLevel);
    return mSpaceStationLevel;
}

void SpaceStationActor::SetRadiusMarkerComponent(const std::shared_ptr<StaticMeshComponent>& radiusMarkerComponent)
{
    assert(!mRadiusMarkerComponent);
    mRadiusMarkerComponent = radiusMarkerComponent;
    AddComponent(radiusMarkerComponent);
}
} // namespace Game
