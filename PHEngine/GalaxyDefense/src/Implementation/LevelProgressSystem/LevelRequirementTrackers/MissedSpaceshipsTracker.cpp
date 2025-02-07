#include "MissedSpaceshipsTracker.h"

#include "Implementation/DataProviders/LevelDataProvider.h"

namespace Game {
MissedSpaceshipsTracker::MissedSpaceshipsTracker(const int32_t missSpaceshipsCountToFailTracker)
    : mMissSpaceshipsCountToFailTracker(missSpaceshipsCountToFailTracker)
{
}

void MissedSpaceshipsTracker::Init()
{
    mInitialMissedSpaceshipsCount = LevelDataProvider::GetInstance()->GetCurrentStageSurvivedEnemySpaceshipsCount();
}

void MissedSpaceshipsTracker::SetMissedSpaceshipsCount(const int32_t value)
{
    if (mMissedSpaceshipsCount != value) {
        mLastMissedSpaceshipsCount = mMissedSpaceshipsCount;
        mMissedSpaceshipsCount = value;
    }
}

void MissedSpaceshipsTracker::Tick(const float deltaTime)
{
    SetMissedSpaceshipsCount(
        LevelDataProvider::GetInstance()->GetCurrentStageSurvivedEnemySpaceshipsCount() - mInitialMissedSpaceshipsCount);
}

bool MissedSpaceshipsTracker::IsRequirementAchived() const
{
    return false;
}

bool MissedSpaceshipsTracker::IsRequirementFailed() const
{
    return mMissedSpaceshipsCount > mMissSpaceshipsCountToFailTracker;
}

eLevelRequirementTrackerType MissedSpaceshipsTracker::GetType() const
{
    return eLevelRequirementTrackerType::NEED_NOT_TO_FAIL;
}

std::string MissedSpaceshipsTracker::GetName() const
{
    return "MissedSpaceshipsTracker";
}

std::unordered_map<std::string, std::string> MissedSpaceshipsTracker::SerializeParameters() const
{
    std::unordered_map<std::string, std::string> result;
    result["name"] = GetName();
    result["not_to_miss_spaceships_count"] = std::to_string(mMissSpaceshipsCountToFailTracker);
    result["missed_spaceships_count"] = std::to_string(mMissedSpaceshipsCount);
    return result;
}

bool MissedSpaceshipsTracker::CheckIfRequirementIsDirty(const bool unsetDirtyFlag)
{
    if (!IsRequirementFailed() && mLastMissedSpaceshipsCount != mMissedSpaceshipsCount) {
        mLastMissedSpaceshipsCount = unsetDirtyFlag ? mMissedSpaceshipsCount : mLastMissedSpaceshipsCount;
        return true;
    }
    return false;
}
} // namespace Game
