#pragma once

#include <memory>
#include <string>
#include <json/json.hpp>

namespace Game
{
    class ILevelRequirementTracker;

    class ILevelRequirementTrackerFactory
    {
    public:
        virtual std::unique_ptr<ILevelRequirementTracker> CreateLevelRequirementTracker(const std::string &requirementTrackerType,
                                                                                        const nlohmann::json &trackerRootJson) const = 0;
    };
}
