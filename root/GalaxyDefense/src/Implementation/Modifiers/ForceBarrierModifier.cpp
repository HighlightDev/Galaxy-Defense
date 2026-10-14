#include "ForceBarrierModifier.h"

#include "Core/CommonCore/Assertion.h"
#include "Implementation/Actors/BarrierActor.h"

namespace Game {
ForceBarrierModifier::ForceBarrierModifier(const std::weak_ptr<BarrierActor>& owner)
    : mOwnerWp(owner)
{
}

eModifierType ForceBarrierModifier::GetModifierType() const
{
    return eModifierType::ForceBarrier;
}

int32_t ForceBarrierModifier::CreatorObjectId() const
{
    const auto& ownerSp = mOwnerWp.lock();
    ext_assert(ownerSp, "ForceBarrierModifier owner barrier pointer is null");
    return ownerSp->GetObjectId();
}

void ForceBarrierModifier::Tick(const float deltaTimeSec, const float playSpeed)
{
    if (const auto& barrierSp = mOwnerWp.lock()) {
        barrierSp->SetForceShieldActive(true);
    }
}

void ForceBarrierModifier::OnPreRemoved()
{
    if (const auto& barrierSp = mOwnerWp.lock()) {
        barrierSp->SetForceShieldActive(false);
    }
}

void ForceBarrierModifier::SetIsExpired(const bool isExpired)
{
    mIsExpired = isExpired;
}

bool ForceBarrierModifier::IsExpired() const
{
    return mIsExpired;
}
} // namespace Game
