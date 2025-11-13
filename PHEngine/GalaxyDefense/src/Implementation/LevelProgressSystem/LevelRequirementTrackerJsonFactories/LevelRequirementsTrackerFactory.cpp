#include "LevelRequirementsTrackerFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/JsonUtilities.h"
#include "Implementation/LevelProgressSystem/LevelRequirementTrackers/DestroySpaceshipsTracker.h"
#include "Implementation/LevelProgressSystem/LevelRequirementTrackers/ILevelRequirementTracker.h"
#include "Implementation/LevelProgressSystem/LevelRequirementTrackers/MissedSpaceshipsTracker.h"

namespace Game {
std::unique_ptr<ILevelRequirementTracker> LevelRequirementsTrackerFactory::CreateLevelRequirementTracker(
    const std::string& requirementTrackerType, const nlohmann::json& trackerRootJson) const
{
    if ("DestroySpaceshipsTracker" == requirementTrackerType) {
        const auto enemiesCount = nlohmann_utilities::GetIntFromJson(trackerRootJson, "spaceships_count");
        const std::string& hint = trackerRootJson.at("hint");
        auto destroySpaceShipsTracker = std::make_unique<DestroySpaceshipsTracker>(enemiesCount, hint);
        return destroySpaceShipsTracker;
    }
    if ("MissedSpaceshipsTracker" == requirementTrackerType) {
        const auto doNotMissSpaceshipsCount = nlohmann_utilities::GetIntFromJson(trackerRootJson, "spaceships_count");
        const std::string& hint = trackerRootJson.at("hint");
        auto destroySpacehipsTracker = std::make_unique<MissedSpaceshipsTracker>(doNotMissSpaceshipsCount, hint);
        return destroySpacehipsTracker;
    } else {
        assert(false);
    }

    return nullptr;
}
} // namespace Game
