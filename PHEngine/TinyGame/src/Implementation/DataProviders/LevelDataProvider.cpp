#include "LevelDataProvider.h"

namespace Game {
LevelDataProvider* LevelDataProvider::GetInstance()
{
    static LevelDataProvider levelDataProvider;
    return &levelDataProvider;
}

int32_t LevelDataProvider::GetCurrentStageAliveEnemySpaceshipsCount() const
{
    return mCurrentStageAliveEnemySpaceshipsCount;
}

void LevelDataProvider::SetCurrentStageAliveEnemySpaceshipsCount(const int32_t currentStageAliveEnemySpaceshipsCount)
{
    mCurrentStageAliveEnemySpaceshipsCount = currentStageAliveEnemySpaceshipsCount;
}
} // namespace Game
