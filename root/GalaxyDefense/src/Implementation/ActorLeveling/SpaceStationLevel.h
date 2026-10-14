#pragma once

#include "Implementation/MissileType.h"

#include <stdint.h>

#include <cstdint>

namespace Game {
class SpaceStationLevel {

    eMissileType mMissileType{eMissileType::NONE};

    int32_t mLvl{1};

    float mShootRadius{10.0f};

    int32_t mCooldownMs{1000};

public:
    SpaceStationLevel() = delete;

    SpaceStationLevel(const eMissileType missileType, const int32_t lvl, const float shootRadius, const int32_t cooldownMs);

    eMissileType GetMissileType() const;

    int32_t GetLvl() const;

    float GetShootRadius() const;

    int32_t GetCooldownMs() const;
};
} // namespace Game