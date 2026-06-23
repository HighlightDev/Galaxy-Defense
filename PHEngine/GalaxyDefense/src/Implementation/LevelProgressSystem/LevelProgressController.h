#pragma once

#include "Core/GameCore/ITickable.h"
#include "LevelProgressStage.h"

#include <memory>
#include <queue>
#include <string>

namespace Game {
class ILevelRequirementTracker;

class LevelProgressController : public ITickable {
    std::shared_ptr<LevelProgressStage> mCurrentStage;

    std::queue<std::shared_ptr<LevelProgressStage>> mLevelProgressStages;

public:
    LevelProgressController();

    void InitNextStage();

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void AddLevelProgressStage(const std::shared_ptr<LevelProgressStage>& lvlProgressStage);

    int32_t GetCurrentProgressRequirementsCount() const;

    std::string GetCurrentProgressStageName() const;

    std::vector<std::shared_ptr<ILevelRequirementTracker>> GetLevelProgressRequirementTrackers() const;
};
} // namespace Game
