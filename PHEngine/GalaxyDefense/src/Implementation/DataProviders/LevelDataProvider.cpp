#include "LevelDataProvider.h"

namespace Game
{
    LevelDataProvider *LevelDataProvider::GetInstance()
    {
        static LevelDataProvider levelDataProvider;
        return &levelDataProvider;
    }

    int32_t LevelDataProvider::GetCurrentStageSurvivedEnemySpaceshipsCount() const
    {
        return mCurrentStageSurvivedEnemySpaceshipsCount;
    }

    void LevelDataProvider::SetCurrentStageSurvivedEnemySpaceshipsCount(const int32_t count)
    {
        mCurrentStageSurvivedEnemySpaceshipsCount = count;
    }
}
