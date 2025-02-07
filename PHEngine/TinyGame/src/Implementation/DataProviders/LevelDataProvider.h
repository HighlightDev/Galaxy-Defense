#pragma once

#include <stdint.h>

#include <cstdint>

namespace Game {
class LevelDataProvider {
    int32_t mCurrentStageAliveEnemySpaceshipsCount{0};

private:
    LevelDataProvider() = default;

public:
    static LevelDataProvider* GetInstance();

    int32_t GetCurrentStageAliveEnemySpaceshipsCount() const;

    void SetCurrentStageAliveEnemySpaceshipsCount(const int32_t currentStageAliveEnemySpaceships);
};
} // namespace Game
