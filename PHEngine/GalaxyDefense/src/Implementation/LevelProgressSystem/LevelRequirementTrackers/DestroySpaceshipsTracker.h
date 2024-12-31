#pragma once

#include "ILevelRequirementTracker.h"

#include <stdint.h>
#include <cstdint>

namespace Game
{
    class DestroySpaceshipsTracker
        : public ILevelRequirementTracker
    {
        const int32_t mEnemySpaceshipsToDestroyCount;

        int32_t mCurrentStageDestroyedEnemySpaceshipsInitialCount;

        int32_t mDestroyedEnemySpaceships;

    public:
        explicit DestroySpaceshipsTracker(const int32_t enemySpaceshipsCount);

        bool IsRequirementAchived() const override;

        std::string GetName() const override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;
    };
}