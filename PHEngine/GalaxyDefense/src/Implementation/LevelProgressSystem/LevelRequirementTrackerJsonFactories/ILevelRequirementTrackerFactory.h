#pragma once

#include <json/json.hpp>

#include <memory>
#include <string>

namespace Game {
class ILevelRequirementTracker;

class ILevelRequirementTrackerFactory {
public:
    virtual ~ILevelRequirementTrackerFactory() = default;

    virtual std::unique_ptr<ILevelRequirementTracker>
    CreateLevelRequirementTracker(const std::string& requirementTrackerType, const nlohmann::json& trackerRootJson) const = 0;
};
} // namespace Game
