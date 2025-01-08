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

        int32_t mCurrentStageDestroyedEnemySpaceshipsInitialCount{0};

        int32_t mDestroyedEnemySpaceships{0};

        int32_t mLastDestroyedEnemySpaceshipsValue{0};

    public:
        explicit DestroySpaceshipsTracker(const int32_t enemySpaceshipsCount);

        void Init() override;

        bool IsRequirementAchived() const override;

        std::string GetName() const override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        std::unordered_map<std::string, std::string> SerializeParameters() const override;

        bool CheckIfRequirementIsDirty(const bool unsetDirtyFlag) override;

    private:
        void SetDestroyedEnemySpaceships(const int32_t value);
    };
}