#include "LevelProgressStage.h"

#include "Core/CommonCore/Assertion.h"

#include <algorithm>

namespace Game {
LevelProgressStage::LevelProgressStage(const std::string& stageName)
    : mStageName(stageName)
{
}

void LevelProgressStage::Init()
{
    for (const auto& requirement : mLevelProgressRequirements) {
        mLevelProgressRequirementTrackers.emplace_back(requirement->CreateRequirementTracker());
    }
}

void LevelProgressStage::AddLevelProgressRequirement(const std::shared_ptr<ILevelProgressRequirement>& lvlProgressRequirement)
{
    mLevelProgressRequirements.emplace_back(lvlProgressRequirement);
}

void LevelProgressStage::Tick(const float deltaTime)
{
    bool isPendingRemovalAchivedRequirements = false;
    for (const auto& requirementTracker : mLevelProgressRequirementTrackers) {
        requirementTracker->Tick(deltaTime);

        isPendingRemovalAchivedRequirements |= requirementTracker->IsRequirementAchived();
    }

    if (isPendingRemovalAchivedRequirements) {
        auto requirementRemoveIt = std::remove_if(
            mLevelProgressRequirements.begin(), mLevelProgressRequirements.end(), [this](const auto& levelProgressRequirement) {
                const auto trackerIt = std::find_if(
                    mLevelProgressRequirementTrackers.cbegin(),
                    mLevelProgressRequirementTrackers.cend(),
                    [requirementName = levelProgressRequirement->ToString()](const auto& tracker) {
                        return tracker->GetName() == requirementName;
                    });
                assert(trackerIt != mLevelProgressRequirementTrackers.cend());
                return (*trackerIt)->IsRequirementAchived();
            });

        mLevelProgressRequirements.erase(requirementRemoveIt, mLevelProgressRequirements.end());

        auto trackerRemoveIt = std::remove_if(
            mLevelProgressRequirementTrackers.begin(), mLevelProgressRequirementTrackers.end(), [](const auto& tracker) {
                return tracker->IsRequirementAchived();
            });
        mLevelProgressRequirementTrackers.erase(trackerRemoveIt);
    }
}

void LevelProgressStage::UnpausableTick(const float deltaTime)
{
}
} // namespace Game
