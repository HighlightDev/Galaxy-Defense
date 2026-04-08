#include "BarrierLevel.h"

#include "Core/CommonCore/Assertion.h"

namespace Game {
BarrierLevel::BarrierLevel(const uint32_t pillarHealth)
    : mNominalPillarHealth(pillarHealth)
    , mCurrentPillarHealth(pillarHealth)
{
    ext_assert(pillarHealth > 0, "BarrierLevel pillar health must be greater than zero");
}

uint32_t BarrierLevel::GetPillarHealth() const
{
    return mCurrentPillarHealth;
}

uint32_t BarrierLevel::GetNominalPillarHealth() const
{
    return mNominalPillarHealth;
}

void BarrierLevel::DecreasePillarHealth(const uint32_t dmg)
{
    if (dmg >= mCurrentPillarHealth) {
        mCurrentPillarHealth = 0;
    } else {
        mCurrentPillarHealth -= dmg;
    }
}

void BarrierLevel::RestorePillarHealth()
{
    mCurrentPillarHealth = mNominalPillarHealth;
}
} // namespace Game
