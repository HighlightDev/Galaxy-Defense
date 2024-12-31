#include "LevelRequirementsTrackerFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include "Implementation/LevelProgressSystem/LevelRequirementTrackers/ILevelRequirementTracker.h"
#include "Implementation/LevelProgressSystem/LevelRequirementTrackers/DestroySpaceshipsTracker.h"

namespace Game
{
    std::unique_ptr<ILevelRequirementTracker>
    LevelRequirementsTrackerFactory::CreateLevelRequirementTracker(const std::string &requirementTrackerType,
                                                                   const nlohmann::json &trackerRootJson) const
    {
        if ("DestroySpaceshipsTracker" == requirementTrackerType)
        {
            const auto enemiesCount = nlohmann_utilities::GetIntFromJson(trackerRootJson.at("spaceships_count"));
            auto destroySpaceShipsTracker = std::make_unique<DestroySpaceshipsTracker>(enemiesCount);
            return destroySpaceShipsTracker;
        }
        else
        {
            assert(false);
        }

        return nullptr;
    }
}
