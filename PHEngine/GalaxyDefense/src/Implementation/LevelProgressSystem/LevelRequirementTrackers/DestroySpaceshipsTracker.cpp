#include "DestroySpaceshipsTracker.h"

#include "Implementation/DataProviders/LevelDataProvider.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"

namespace Game
{

    DestroySpaceshipsTracker::DestroySpaceshipsTracker(const int32_t enemySpaceshipsCount)
        : mEnemySpaceshipsToDestroyCount(enemySpaceshipsCount)
    {
    }

    void DestroySpaceshipsTracker::Init()
    {
        mCurrentStageDestroyedEnemySpaceshipsInitialCount = PlayerDataProvider::GetInstance()->GetDestroyedEnemySpaceshipsCount();
    }

    bool DestroySpaceshipsTracker::IsRequirementAchived() const
    {
        return mDestroyedEnemySpaceships > mEnemySpaceshipsToDestroyCount;
    }

    bool DestroySpaceshipsTracker::IsRequirementFailed() const
    {
        return false;
    }

    eLevelRequirementTrackerType DestroySpaceshipsTracker::GetType() const
    {
        return eLevelRequirementTrackerType::NEED_TO_ACHIVE;
    }

    std::string DestroySpaceshipsTracker::GetName() const
    {
        return "DestroySpaceshipsTracker";
    }

    void DestroySpaceshipsTracker::Tick(const float deltaTime)
    {
        SetDestroyedEnemySpaceships(PlayerDataProvider::GetInstance()->GetDestroyedEnemySpaceshipsCount() - mCurrentStageDestroyedEnemySpaceshipsInitialCount);
    }

    void DestroySpaceshipsTracker::SetDestroyedEnemySpaceships(const int32_t value)
    {
        if (mDestroyedEnemySpaceships != value)
        {
            mLastDestroyedEnemySpaceshipsValue = mDestroyedEnemySpaceships;
            mDestroyedEnemySpaceships = std::min(value, mEnemySpaceshipsToDestroyCount);
        }
    }

    void DestroySpaceshipsTracker::UnpausableTick(const float deltaTime)
    {
    }

    std::unordered_map<std::string, std::string> DestroySpaceshipsTracker::SerializeParameters() const
    {
        std::unordered_map<std::string, std::string> result;
        result["name"] = GetName();
        result["to_destroy_spaceships_count"] = std::to_string(mEnemySpaceshipsToDestroyCount);
        result["left_to_destroy_spaceships_count"] = std::to_string(std::max(mEnemySpaceshipsToDestroyCount - mDestroyedEnemySpaceships, 0));
        return result;
    }

    bool DestroySpaceshipsTracker::CheckIfRequirementIsDirty(const bool unsetDirtyFlag)
    {
        if (!IsRequirementAchived() &&
            mLastDestroyedEnemySpaceshipsValue != mDestroyedEnemySpaceships)
        {
            mLastDestroyedEnemySpaceshipsValue = unsetDirtyFlag ? mDestroyedEnemySpaceships : mLastDestroyedEnemySpaceshipsValue;
            return true;
        }
        return false;
    }
}