#include "SpaceStationLevel.h"

namespace Game {
SpaceStationLevel::SpaceStationLevel(
    const eMissileType missileType, const int32_t lvl, const float shootRadius, const int32_t cooldownMs)
    : mMissileType(missileType)
    , mLvl(lvl)
    , mShootRadius(shootRadius)
    , mCooldownMs(cooldownMs)
{
}

eMissileType SpaceStationLevel::GetMissileType() const
{
    return mMissileType;
}

int32_t SpaceStationLevel::GetLvl() const
{
    return mLvl;
}

float SpaceStationLevel::GetShootRadius() const
{
    return mShootRadius;
}

int32_t SpaceStationLevel::GetCooldownMs() const
{
    return mCooldownMs;
}

} // namespace Game