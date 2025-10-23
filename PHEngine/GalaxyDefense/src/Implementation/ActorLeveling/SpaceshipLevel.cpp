#include "SpaceshipLevel.h"

namespace Game {
SpaceshipLevel::SpaceshipLevel(const uint32_t health)
    : mNominalHealth(health)
    , mCurrentHealth(health)
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

} // namespace Game