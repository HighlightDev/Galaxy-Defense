#pragma once

#include "Implementation/MissileType.h"
#include "SpaceshipLevel.h"

#include <cstdint>

namespace Game {
class FighterSpaceshipLevel : public SpaceshipLevel {

    eMissileType mMissileType{eMissileType::NONE};

    int32_t mLvl{1};

    float mShootRadius{10.0f};

    int32_t mCooldownMs{1000};

public:
    FighterSpaceshipLevel() = delete;

    FighterSpaceshipLevel(
        const uint32_t health,
        const eMissileType missileType,
        const int32_t lvl,
        const float shootRadius,
        const int32_t cooldownMs);

    eMissileType GetMissileType() const;

    int32_t GetLvl() const;

    float GetShootRadius() const;

    int32_t GetCooldownMs() const;
};
} // namespace Game
