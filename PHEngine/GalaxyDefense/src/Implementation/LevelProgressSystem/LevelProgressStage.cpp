#include "LevelProgressStage.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

namespace Game
{
    LevelProgressStage::LevelProgressStage(const std::string &stageName)
        : mStageName(stageName)
    {
    }

    void LevelProgressStage::Init()
    {
    }

    void LevelProgressStage::AddLevelProgressRequirementTracker(const std::shared_ptr<ILevelRequirementTracker> &lvlReqTracker)
    {
        mLevelProgressRequirementTrackers.emplace_back(lvlReqTracker);
    }

    void LevelProgressStage::Tick(const float deltaTime)
    {
        mTotalAchivedReqTrackers = 0;
        for (const auto &requirementTracker : mLevelProgressRequirementTrackers)
        {
            requirementTracker->Tick(deltaTime);
            mTotalAchivedReqTrackers += requirementTracker->IsRequirementAchived() ? 1 : 0;
        }
    }

    void LevelProgressStage::UnpausableTick(const float deltaTime)
    {
    }

    bool LevelProgressStage::IsStageCompleted() const
    {
        return mTotalAchivedReqTrackers == static_cast<int32_t>(mLevelProgressRequirementTrackers.size());
    }

    std::string LevelProgressStage::GetName() const
    {
        return mStageName;
    }
} // namespace Game
