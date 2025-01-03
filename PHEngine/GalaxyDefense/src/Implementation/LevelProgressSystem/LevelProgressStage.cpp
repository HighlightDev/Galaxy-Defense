#include "LevelProgressStage.h"
#include "Core/CommonCore/Assertion.h"

#include "Implementation/Events/LevelProgressChangedEvent.h"

#include <algorithm>
#include <json/json.hpp>

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
        bool requirementsUpdated = false;
        for (const auto &requirementTracker : mLevelProgressRequirementTrackers)
        {
            requirementTracker->Tick(deltaTime);
            mTotalAchivedReqTrackers += requirementTracker->IsRequirementAchived() ? 1 : 0;
            requirementsUpdated |= requirementTracker->CheckIfRequirementIsDirty(true);
        }

        if (requirementsUpdated)
        {
            nlohmann::json jsonObj;
            jsonObj["level_progress_status_type"] = static_cast<int32_t>(eLevelProgressStatusType::REQUIREMENT_TRACKERS_STATUS_CHANGED);
            const auto &eventParams = jsonObj.dump();
            Event::LuaLevelProgressChangedEvent::GetInstance()->SendEvent(Event::eExecutionOrder::POST_EXECUTION,
                                                                          eLevelProgressStatusType::REQUIREMENT_TRACKERS_STATUS_CHANGED,
                                                                          eventParams);
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

    int32_t LevelProgressStage::GetProgressRequirementsCount() const
    {
        return static_cast<int32_t>(mLevelProgressRequirementTrackers.size());
    }

    std::vector<std::shared_ptr<ILevelRequirementTracker>> LevelProgressStage::GetLevelProgressRequirementTrackers() const
    {
        return mLevelProgressRequirementTrackers;
    }
} // namespace Game
