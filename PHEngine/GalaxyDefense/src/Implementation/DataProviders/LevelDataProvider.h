#pragma once

#include <stdint.h>

#include <cstdint>

namespace Game {
class LevelDataProvider {
    int32_t mCurrentStageSurvivedEnemySpaceshipsCount{0};

private:
    LevelDataProvider() = default;

public:
    static LevelDataProvider* GetInstance();

    int32_t GetCurrentStageSurvivedEnemySpaceshipsCount() const;

    void SetCurrentStageSurvivedEnemySpaceshipsCount(const int32_t count);
};
} // namespace Game
