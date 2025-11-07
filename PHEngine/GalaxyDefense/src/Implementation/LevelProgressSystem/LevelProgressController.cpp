#include "LevelProgressController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Implementation/Events/LevelProgressChangedEvent.h"

#include <json/json.hpp>

using namespace EngineCore;

namespace Game {
LevelProgressController::LevelProgressController()
    : mCurrentStage()
{
}

void LevelProgressController::InitNextStage()
{
    bool sendEvent = false;
    if (mLevelProgressStages.size()) {
        mCurrentStage = mLevelProgressStages.front();
        mLevelProgressStages.pop();
        mCurrentStage->Init();
        sendEvent = true;
    } else if (mCurrentStage) {
        mCurrentStage = nullptr;
        sendEvent = true;
    }

    if (sendEvent) {
        nlohmann::json jsonObj;
        jsonObj["level_progress_status_type"] = static_cast<int32_t>(eLevelProgressStatusType::CURRENT_STAGE_CHANGED);
        const auto& eventParams = jsonObj.dump();
        Event::LuaLevelProgressChangedEvent::GetInstance()->SendEvent(
            Event::eExecutionOrder::POST_EXECUTION, eLevelProgressStatusType::CURRENT_STAGE_CHANGED, eventParams);
    }
}

void LevelProgressController::AddLevelProgressStage(const std::shared_ptr<LevelProgressStage>& lvlProgressStage)
{
    assert(!mCurrentStage);
    mLevelProgressStages.emplace(lvlProgressStage);
}

void LevelProgressController::Tick(const float deltaTimeSec)
{
    if (mCurrentStage) {
        mCurrentStage->Tick(deltaTimeSec);

        if (mCurrentStage->IsStageCompleted()) {
            LogInfo("LevelProgressController::Stage ", mCurrentStage->GetName(), " completed. Going to next stage.");
            InitNextStage();
        }
    } else {
        InitNextStage();
    }
}

void LevelProgressController::UnpausableTick(const float deltaTimeSec)
{
}

int32_t LevelProgressController::GetCurrentProgressRequirementsCount() const
{
    if (mCurrentStage) {
        return mCurrentStage->GetProgressRequirementsCount();
    }

    return 0;
}

std::string LevelProgressController::GetCurrentProgressStageName() const
{
    if (mCurrentStage) {
        return mCurrentStage->GetName();
    }

    return "";
}

std::vector<std::shared_ptr<ILevelRequirementTracker>> LevelProgressController::GetLevelProgressRequirementTrackers() const
{
    if (mCurrentStage) {
        return mCurrentStage->GetLevelProgressRequirementTrackers();
    }

    return {};
}
} // namespace Game
