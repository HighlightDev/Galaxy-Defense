#include "FreezingModifier.h"

#include "Implementation/Actors/SpaceshipActor.h"

namespace Game {
FreezingModifier::FreezingModifier(const std::weak_ptr<SpaceshipActor>& owner)
    : mOwnerWp(owner)
    , mFreezingPower(1.0f)
    , mFreezingTimer(0.0f)
    , mFreezingTimeout(1.0f)
{
}

eModifierType FreezingModifier::GetModifierType() const
{
    return eModifierType::Freezing;
}

int32_t FreezingModifier::CreatorObjectId() const
{
    const auto& ownerSp = mOwnerWp.lock();
    assert(ownerSp);
    return ownerSp->GetObjectId();
}

void FreezingModifier::Tick(const float deltaTime)
{
    mFreezingTimer += deltaTime;

    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        assert(movementComponent);
        const float referenceSpeed = movementComponent->GetReferenceSpeed();
        movementComponent->SetCurrentSpeed(referenceSpeed / mFreezingPower);

        spaceshipSp->SetFreezingEffectValue(glm::clamp(mFreezingTimer / mFreezingTimeout, 0.0f, 1.0f));
    }
}

void FreezingModifier::OnPreRemoved()
{
    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        assert(movementComponent);
        movementComponent->SetCurrentSpeedToReferenceValue();

        spaceshipSp->SetFreezingEffectValue(0.0f);
    }
}

bool FreezingModifier::IsExpired() const
{
    return mFreezingTimer >= mFreezingTimeout;
}

void FreezingModifier::SetFreezingPower(const float power)
{
    assert(power > 0.0001f);
    mFreezingPower = power;
}

void FreezingModifier::ResetFreezingTimer()
{
    mFreezingTimer = 0.0f;
}

void FreezingModifier::SetFreezingTimeout(const float timeout)
{
    mFreezingTimeout = timeout;
}

float FreezingModifier::GetFreezingTimer() const
{
    return mFreezingTimer;
}
} // namespace Game