#include "DestroySpaceshipsTracker.h"

#include "Implementation/DataProviders/LevelDataProvider.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"

namespace Game
{

    DestroySpaceshipsTracker::DestroySpaceshipsTracker(const int32_t enemySpaceshipsCount)
        : mEnemySpaceshipsToDestroyCount(enemySpaceshipsCount)
    {
        mCurrentStageDestroyedEnemySpaceshipsInitialCount = PlayerDataProvider::GetInstance()->GetDestroyedEnemySpaceshipsCount();
    }

    bool DestroySpaceshipsTracker::IsRequirementAchived() const
    {
        return mDestroyedEnemySpaceships >= mEnemySpaceshipsToDestroyCount;
    }

    std::string DestroySpaceshipsTracker::GetName() const
    {
        return "DestroySpaceshipsTracker";
    }

    void DestroySpaceshipsTracker::Tick(const float deltaTime)
    {
        mDestroyedEnemySpaceships = PlayerDataProvider::GetInstance()->GetDestroyedEnemySpaceshipsCount() - mCurrentStageDestroyedEnemySpaceshipsInitialCount;
    }

    void DestroySpaceshipsTracker::UnpausableTick(const float deltaTime)
    {
    }
}