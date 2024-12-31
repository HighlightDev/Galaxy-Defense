#include "LevelProgressController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Game
{
    LevelProgressController::LevelProgressController()
        : mCurrentStage()
    {
    }

    void LevelProgressController::InitNextStage()
    {
        mCurrentStage = nullptr;
        if (mLevelProgressStages.size())
        {
            mCurrentStage = mLevelProgressStages.front();
            mLevelProgressStages.pop();
            mCurrentStage->Init();
        }
    }

    void LevelProgressController::AddLevelProgressStage(const std::shared_ptr<LevelProgressStage> &lvlProgressStage)
    {
        assert(!mCurrentStage);
        mLevelProgressStages.emplace(lvlProgressStage);
    }

    void LevelProgressController::Tick(const float deltaTime)
    {
        if (mCurrentStage)
        {
            mCurrentStage->Tick(deltaTime);

            if (mCurrentStage->IsStageCompleted())
            {
                LogInfo("LevelProgressController::Stage ", mCurrentStage->GetName(), " completed. Going to next stage.");
                InitNextStage();
            }
        }
    }

    void LevelProgressController::UnpausableTick(const float deltaTime)
    {
    }
}
