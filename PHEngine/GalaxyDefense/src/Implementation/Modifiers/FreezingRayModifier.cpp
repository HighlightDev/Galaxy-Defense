#include "FreezingRayModifier.h"

#include "Implementation/Actors/SpaceshipActor.h"

namespace Game {
FreezingRayModifier::FreezingRayModifier(const std::weak_ptr<SpaceshipActor>& owner)
    : mOwnerWp(owner)
    , mFreezingPower(1.0f)
{
}

eModifierType FreezingRayModifier::GetModifierType() const
{
    return eModifierType::FreezingRay;
}

int32_t FreezingRayModifier::CreatorObjectId() const
{
    const auto& ownerSp = mOwnerWp.lock();
    ext_assert(ownerSp, "FreezingRayModifier owner spaceship pointer is null");
    return ownerSp->GetObjectId();
}

void FreezingRayModifier::Tick(const float deltaTimeSec, const float playSpeed)
{
    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        ext_assert(movementComponent, "FreezingRayModifier spaceship movement component is null");
        const float referenceSpeed = movementComponent->GetReferenceSpeed();
        movementComponent->SetCurrentSpeed(referenceSpeed / mFreezingPower);
        spaceshipSp->SetFreezingEffectValue(1.0f);
    }
}

void FreezingRayModifier::OnPreRemoved()
{
    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        ext_assert(movementComponent, "FreezingRayModifier spaceship movement component is null in OnPreRemoved");
        movementComponent->SetCurrentSpeedToReferenceValue();
        spaceshipSp->SetFreezingEffectValue(0.0f);
    }
}

void FreezingRayModifier::SetIsExpired(const bool isExpired)
{
    mIsExpired = isExpired;
}

bool FreezingRayModifier::IsExpired() const
{
    return mIsExpired;
}

void FreezingRayModifier::SetFreezingPower(const float power)
{
    ext_assert(power > 0.0001f, "FreezingRayModifier freezing power must be greater than 0.0001");
    mFreezingPower = power;
}
} // namespace Game