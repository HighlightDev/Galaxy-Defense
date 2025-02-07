#pragma once

#include "ILevelRequirementTrackerFactory.h"

namespace Game {
class ILevelRequirementTracker;

class LevelRequirementsTrackerFactory : public ILevelRequirementTrackerFactory {
public:
    std::unique_ptr<ILevelRequirementTracker> CreateLevelRequirementTracker(
        const std::string& requirementTrackerType, const nlohmann::json& trackerRootJson) const override;
};
} // namespace Game
