#include "PlasmaModifier.h"

#include "Core/CommonCore/Assertion.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"

#include <cmath>

namespace Game {
PlasmaModifier::PlasmaModifier(const std::weak_ptr<SpaceshipActor>& owner, const std::weak_ptr<MissileActor>& missile)
    : mOwnerWp(owner)
    , mMissileWp(missile)
    , mDamagePerSecond(5.0f)
{
}

eModifierType PlasmaModifier::GetModifierType() const
{
    return eModifierType::Plasma;
}

int32_t PlasmaModifier::CreatorObjectId() const
{
    // The creator is the missile that applied the burn; HasModifier/RemoveModifier are keyed by the missile's object id.
    const auto& missileSp = mMissileWp.lock();
    ext_assert(missileSp, "PlasmaModifier missile pointer is null");
    return missileSp->GetObjectId();
}

void PlasmaModifier::Tick(const float deltaTimeSec, const float playSpeed)
{
    // The burn is gameplay time, so its duration and damage cadence advance with game speed.
    const float scaledDeltaTimeSec = deltaTimeSec * playSpeed;
    mElapsedSec += scaledDeltaTimeSec;

    const auto& spaceshipSp = mOwnerWp.lock();
    const auto& missileSp = mMissileWp.lock();
    if (spaceshipSp && missileSp) {
        // Damage-over-time: accumulate fractional HP and apply whole points so the burn reads as continuous.
        mDamageAccumulator += mDamagePerSecond * scaledDeltaTimeSec;
        if (mDamageAccumulator >= 1.0f) {
            const auto wholeDamage = static_cast<size_t>(std::floor(mDamageAccumulator));
            mDamageAccumulator -= static_cast<float>(wholeDamage);
            spaceshipSp->TriggerDamageReceived(wholeDamage, missileSp->GetDamageDealerType());
        }
    }
}

void PlasmaModifier::OnPreRemoved()
{
}

void PlasmaModifier::SetDamagePerSecond(const float damagePerSecond)
{
    mDamagePerSecond = damagePerSecond;
}

void PlasmaModifier::SetDurationSec(const float durationSec)
{
    mDurationSec = durationSec;
}

bool PlasmaModifier::IsExpired() const
{
    // The burn is its own timer (it outlives the projectile). Also drop it if the ship or missile is gone.
    if (mOwnerWp.expired() || mMissileWp.expired()) {
        return true;
    }
    const auto& ownerSp = mOwnerWp.lock();
    const bool ownerShipInactive = ownerSp->GetSpaceshipActivityState() == eSpaceshipActivityState::IDLE;
    return ownerShipInactive || mElapsedSec >= mDurationSec;
}
} // namespace Game
