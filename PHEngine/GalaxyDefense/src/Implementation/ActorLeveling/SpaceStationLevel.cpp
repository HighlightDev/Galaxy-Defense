#include "SpaceStationLevel.h"

namespace Game {
SpaceStationLevel::SpaceStationLevel(const eMissileType missileType, const int32_t lvl, const float shootRadius)
    : mMissileType(missileType)
    , mLvl(lvl)
    , mShootRadius(shootRadius)
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

} // namespace Game