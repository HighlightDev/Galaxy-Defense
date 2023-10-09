#pragma once

#include "ILevelProgressRequirement.h"
#include "ILevelRequirementTracker.h"
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

        std::vector<std::shared_ptr<ILevelProgressRequirement>> mLevelProgressRequirements;

        std::vector<std::shared_ptr<ILevelRequirementTracker>> mLevelProgressRequirementTrackers;

    public:
        explicit LevelProgressStage(const std::string& stageName);

        void Init();

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void AddLevelProgressRequirement(const std::shared_ptr<ILevelProgressRequirement> &lvlProgressRequirement);
    };
} // namespace Game
