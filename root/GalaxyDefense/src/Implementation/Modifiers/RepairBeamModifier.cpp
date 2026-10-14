#include "RepairBeamModifier.h"

#include "Core/CommonCore/Assertion.h"
#include "Implementation/Actors/BarrierActor.h"

namespace Game {
namespace {
constexpr float c_healPerSecond = 20.0f; // barrier pillar HP restored per second while the beam is connected
} // namespace

RepairBeamModifier::RepairBeamModifier(const std::weak_ptr<BarrierActor>& owner)
    : mOwnerWp(owner)
{
}

eModifierType RepairBeamModifier::GetModifierType() const
{
    return eModifierType::RepairBeam;
}

int32_t RepairBeamModifier::CreatorObjectId() const
{
    const auto& ownerSp = mOwnerWp.lock();
    ext_assert(ownerSp, "RepairBeamModifier owner barrier pointer is null");
    return ownerSp->GetObjectId();
}

void RepairBeamModifier::Tick(const float deltaTimeSec, const float playSpeed)
{
    if (const auto& barrierSp = mOwnerWp.lock()) {
        mHealAccumulator += c_healPerSecond * deltaTimeSec * playSpeed;
        const uint32_t wholeHeal = static_cast<uint32_t>(mHealAccumulator);
        if (wholeHeal > 0) {
            barrierSp->HealMostDamagedPillar(wholeHeal);
            mHealAccumulator -= static_cast<float>(wholeHeal);
        }
    }
}

void RepairBeamModifier::OnPreRemoved()
{
    // The applied healing is permanent; nothing to revert.
}

void RepairBeamModifier::SetIsExpired(const bool isExpired)
{
    mIsExpired = isExpired;
}

bool RepairBeamModifier::IsExpired() const
{
    return mIsExpired;
}
} // namespace Game
