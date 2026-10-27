#pragma once

#include <stdint.h>

#include <cstdint>

namespace Game {
class SpaceshipLevel {

    uint32_t mNominalHealth;

    uint32_t mCurrentHealth;

    uint32_t mSpaceshipDanger;

public:
    SpaceshipLevel() = delete;

    SpaceshipLevel(const uint32_t health, const uint32_t danger);

    uint32_t GetHealth() const;

    uint32_t GetNominalHealth() const;

    void DecreaseHealth(const uint32_t dmg);

    void RestoreHealth();

    uint32_t GetSpaceshipDanger() const;
};
} // namespace Game