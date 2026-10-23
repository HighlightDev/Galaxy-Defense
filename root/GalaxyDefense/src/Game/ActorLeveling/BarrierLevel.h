#pragma once

#include <cstdint>

namespace Game {
class BarrierLevel {
    uint32_t mNominalPillarHealth;
    uint32_t mCurrentPillarHealth;

public:
    BarrierLevel() = delete;

    explicit BarrierLevel(const uint32_t pillarHealth);

    uint32_t GetPillarHealth() const;

    uint32_t GetNominalPillarHealth() const;

    void DecreasePillarHealth(const uint32_t dmg);

    void IncreasePillarHealth(const uint32_t heal);

    void RestorePillarHealth();
};
} // namespace Game
