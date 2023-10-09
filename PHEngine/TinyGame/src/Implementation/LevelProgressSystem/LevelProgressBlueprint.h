#pragma once

#include "LevelProgressStage.h"
#include "Core/GameCore/ITickable.h"

#include <queue>
#include <memory>
#include <string>

namespace Game
{
    class LevelProgressBlueprint
        : public ITickable
    {
        std::shared_ptr<LevelProgressStage> mCurrentStage;

        std::queue<std::shared_ptr<LevelProgressStage>> mLevelProgressStages;

    public:
        LevelProgressBlueprint();

        void Init();

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void AddLevelProgressStage(const std::shared_ptr<LevelProgressStage> &lvlProgressStage);
    };
}
