#include "LevelProgressBlueprint.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{
    LevelProgressBlueprint::LevelProgressBlueprint()
        : mCurrentStage()
    {
    }

    void LevelProgressBlueprint::Init()
    {
        assert(mLevelProgressStages.size());
        mCurrentStage = mLevelProgressStages.front();
        mCurrentStage->Init();
    }

    void LevelProgressBlueprint::AddLevelProgressStage(const std::shared_ptr<LevelProgressStage> &lvlProgressStage)
    {
        mLevelProgressStages.emplace(lvlProgressStage);
    }

    void LevelProgressBlueprint::Tick(const float deltaTime)
    {
        if (mCurrentStage)
        {
            mCurrentStage->Tick(deltaTime);
        }
    }

    void LevelProgressBlueprint::UnpausableTick(const float deltaTime)
    {
    }
}
