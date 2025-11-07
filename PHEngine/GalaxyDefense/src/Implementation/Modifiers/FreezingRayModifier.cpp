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
    assert(ownerSp);
    return ownerSp->GetObjectId();
}

void FreezingRayModifier::Tick(const float deltaTimeSec)
{
    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        assert(movementComponent);
        const float referenceSpeed = movementComponent->GetReferenceSpeed();
        movementComponent->SetCurrentSpeed(referenceSpeed / mFreezingPower);
        spaceshipSp->SetFreezingEffectValue(1.0f);
    }
}

void FreezingRayModifier::OnPreRemoved()
{
    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        assert(movementComponent);
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
    assert(power > 0.0001f);
    mFreezingPower = power;
}
} // namespace Game