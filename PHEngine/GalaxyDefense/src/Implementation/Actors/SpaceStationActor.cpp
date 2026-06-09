#include "SpaceStationActor.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"

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
            SetIsRadiusMarkerActive(false);
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
    ext_assert(spaceStationLevel, "SpaceStationActor space station level pointer is null");
    mSpaceStationLevel = spaceStationLevel;
    const float shootRadius = mSpaceStationLevel->GetShootRadius();
    mShootRadiusProperty->SetValue(shootRadius);
    mRadiusMarkerComponent->SetScale(glm::vec3(shootRadius * 2.0f, 1.0f, shootRadius * 2.0f));
}

const std::shared_ptr<SpaceStationLevel>& SpaceStationActor::GetSpaceStationLevel() const
{
    ext_assert(mSpaceStationLevel, "SpaceStationActor space station level is null");
    return mSpaceStationLevel;
}

void SpaceStationActor::SetMainMeshComponent(const std::shared_ptr<StaticMeshComponent>& mainMeshComponent)
{
    ext_assert(!mMainMeshComponent, "SpaceStationActor main mesh component already exists");
    mMainMeshComponent = mainMeshComponent;
    AddComponent(mainMeshComponent);
}

void SpaceStationActor::SetRadiusMarkerComponent(const std::shared_ptr<StaticMeshComponent>& radiusMarkerComponent)
{
    ext_assert(!mRadiusMarkerComponent, "SpaceStationActor radius marker component already exists");
    mRadiusMarkerComponent = radiusMarkerComponent;
    AddComponent(radiusMarkerComponent);
}

void SpaceStationActor::SetSpaceStationSize(const glm::vec3& size)
{
    mSpaceStationSize = size;
}

const glm::vec3& SpaceStationActor::GetSpaceStationSize() const
{
    return mSpaceStationSize;
}

void SpaceStationActor::SetIsEnabled(bool isEnabled)
{
    // The radius marker's enabled state is managed independently via SetIsRadiusMarkerActive.
    // Preserve it so that Actor::SetIsEnabled (which propagates to all components) does not override it.
    const bool radiusMarkerEnabled = mRadiusMarkerComponent && mRadiusMarkerComponent->IsEnabled();
    Actor::SetIsEnabled(isEnabled);
    if (mRadiusMarkerComponent) {
        mRadiusMarkerComponent->SetIsEnabled(radiusMarkerEnabled);
    }
}

void SpaceStationActor::SetIsOutlineApplied(const bool isOutlineApplied)
{
    if (mMainMeshComponent) {
        mMainMeshComponent->SetIsOutlineApplied(isOutlineApplied);
    }
}

bool SpaceStationActor::GetIsOutlineApplied() const
{
    if (mMainMeshComponent) {
        return mMainMeshComponent->GetIsOutlineApplied();
    }
    return false;
}

void SpaceStationActor::ChangeHighlightState(const bool isHighlightEnabled)
{
    constexpr int32_t cNoObject = -1;
    SetIsOutlineApplied(isHighlightEnabled);
    SetIsRadiusMarkerActive(isHighlightEnabled);

    if (isHighlightEnabled) {
        PlayerDataProvider::GetInstance()->SetSelectedTower(GetObjectId(), mSpaceStationLevel->GetMissileType());
    } else {
        PlayerDataProvider::GetInstance()->SetSelectedTower(cNoObject, eMissileType::NONE);
    }
}

void SpaceStationActor::SetIsRadiusMarkerActive(const bool isRadiusMarkerActive)
{
    if (mRadiusMarkerComponent) {
        mRadiusMarkerComponent->SetIsEnabled(isRadiusMarkerActive);
    }
}

bool SpaceStationActor::GetIsRadiusMarkerActive() const
{
    if (mRadiusMarkerComponent) {
        return mRadiusMarkerComponent->IsEnabled();
    }
    return false;
}
} // namespace Game
