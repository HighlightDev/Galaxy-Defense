#pragma once

#include <stdint.h>

#include <cstdint>

namespace Game {
class SpaceshipLevel {

    uint32_t mNominalHealth;

    uint32_t mCurrentHealth;

public:
    SpaceshipLevel() = delete;

    SpaceshipLevel(const uint32_t health);

    uint32_t GetHealth() const;

    uint32_t GetNominalHealth() const;

    void DecreaseHealth(const uint32_t dmg);

    void RestoreHealth();
};
} // namespace Game