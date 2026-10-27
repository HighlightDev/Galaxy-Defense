#include "SpaceshipLevel.h"

namespace Game {
SpaceshipLevel::SpaceshipLevel(const uint32_t health, const uint32_t danger)
    : mNominalHealth(health)
    , mCurrentHealth(health)
    , mSpaceshipDanger(danger)
{
}

uint32_t SpaceshipLevel::GetHealth() const
{
    return mCurrentHealth;
}

uint32_t SpaceshipLevel::GetNominalHealth() const
{
    return mNominalHealth;
}

void SpaceshipLevel::DecreaseHealth(const uint32_t dmg)
{
    if (dmg >= mCurrentHealth) {
        mCurrentHealth = 0;
    } else {
        mCurrentHealth -= dmg;
    }
}

void SpaceshipLevel::RestoreHealth()
{
    mCurrentHealth = mNominalHealth;
}

uint32_t SpaceshipLevel::GetSpaceshipDanger() const
{
    return mSpaceshipDanger;
}

} // namespace Game