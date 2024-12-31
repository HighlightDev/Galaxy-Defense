#pragma once

#include "LevelProgressStage.h"
#include "Core/GameCore/ITickable.h"

#include <queue>
#include <memory>
#include <string>

namespace Game
{
    class LevelProgressController
        : public ITickable
    {
        std::shared_ptr<LevelProgressStage> mCurrentStage;

        std::queue<std::shared_ptr<LevelProgressStage>> mLevelProgressStages;

    public:
        LevelProgressController();

        void InitNextStage();

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void AddLevelProgressStage(const std::shared_ptr<LevelProgressStage> &lvlProgressStage);
    };
}
