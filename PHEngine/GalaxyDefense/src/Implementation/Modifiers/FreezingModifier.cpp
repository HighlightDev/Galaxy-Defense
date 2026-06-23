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
    ext_assert(ownerSp, "FreezingModifier owner spaceship pointer is null");
    return ownerSp->GetObjectId();
}

void FreezingModifier::Tick(const float deltaTimeSec, const float playSpeed)
{
    // Freeze build-up/duration is gameplay time, so it advances with game speed.
    mFreezingTimer += deltaTimeSec * playSpeed;

    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        ext_assert(movementComponent, "FreezingModifier spaceship movement component is null");
        const float referenceSpeed = movementComponent->GetReferenceSpeed();
        movementComponent->SetCurrentSpeed(referenceSpeed / mFreezingPower);

        spaceshipSp->SetFreezingEffectValue(glm::clamp(mFreezingTimer / mFreezingTimeout, 0.0f, 1.0f));
    }
}

void FreezingModifier::OnPreRemoved()
{
    if (const auto& spaceshipSp = mOwnerWp.lock()) {
        const auto& movementComponent = spaceshipSp->GetMovementComponent();
        ext_assert(movementComponent, "FreezingModifier spaceship movement component is null in OnPreRemoved");
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
    ext_assert(power > 0.0001f, "FreezingModifier freezing power must be greater than 0.0001");
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