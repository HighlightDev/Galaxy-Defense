#include "FighterSpaceshipLevel.h"

namespace Game {
FighterSpaceshipLevel::FighterSpaceshipLevel(
    const uint32_t health, const eMissileType missileType, const int32_t lvl, const float shootRadius, const int32_t cooldownMs)
    : SpaceshipLevel(health)
    , mMissileType(missileType)
    , mLvl(lvl)
    , mShootRadius(shootRadius)
    , mCooldownMs(cooldownMs)
{
}

eMissileType FighterSpaceshipLevel::GetMissileType() const
{
    return mMissileType;
}

int32_t FighterSpaceshipLevel::GetLvl() const
{
    return mLvl;
}

float FighterSpaceshipLevel::GetShootRadius() const
{
    return mShootRadius;
}

int32_t FighterSpaceshipLevel::GetCooldownMs() const
{
    return mCooldownMs;
}
} // namespace Game
