#pragma once

#include "Implementation/MissileType.h"

#include <stdint.h>

#include <cstdint>

namespace Game {

class LevelAttributeDataProvider {
public:
    virtual ~LevelAttributeDataProvider() = default;

    static int32_t GetCooldownForMissileTypeAtLevel(const eMissileType missileType, const int32_t level);

    static float GetRadiusForMissileTypeAtLevel(const eMissileType missileType, const int32_t level);
};

} // namespace Game