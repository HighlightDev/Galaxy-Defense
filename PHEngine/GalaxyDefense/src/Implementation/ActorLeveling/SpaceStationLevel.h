#pragma once

#include "Implementation/MissileType.h"

#include <stdint.h>

#include <cstdint>

namespace Game {
class SpaceStationLevel {

    eMissileType mMissileType{eMissileType::NONE};

    int32_t mLvl{1};

    float mShootRadius{100.0f};

public:
    SpaceStationLevel() = delete;

    SpaceStationLevel(const eMissileType missileType, const int32_t lvl, const float shootRadius);

    eMissileType GetMissileType() const;

    int32_t GetLvl() const;

    float GetShootRadius() const;
};
} // namespace Game