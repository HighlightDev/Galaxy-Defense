#pragma once

#include "LevelRequirementTrackers/ILevelRequirementTracker.h"
#include "Core/GameCore/ITickable.h"

#include <vector>
#include <memory>
#include <string>

namespace Game
{
    class LevelProgressStage
        : public ITickable
    {
        std::string mStageName;

        std::vector<std::shared_ptr<ILevelRequirementTracker>> mLevelProgressRequirementTrackers;

        int32_t mTotalAchivedReqTrackers{0};

    public:
        explicit LevelProgressStage(const std::string &stageName);

        void Init();

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void AddLevelProgressRequirementTracker(const std::shared_ptr<ILevelRequirementTracker> &lvlReqTracker);

        bool IsStageCompleted() const;

        std::string GetName() const;

        int32_t GetProgressRequirementsCount() const;

        std::vector<std::shared_ptr<ILevelRequirementTracker>> GetLevelProgressRequirementTrackers() const;
    };
} // namespace Game
