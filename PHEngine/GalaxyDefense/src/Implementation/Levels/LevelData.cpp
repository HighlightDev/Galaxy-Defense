#include "LevelData.h"

#include "Core/UtilityCore/EngineMath.h"

namespace Game {
bool LevelData::isDataValid() const
{
    return LevelName != "" && TowersData.size() && SpawnPortalsData.size() && IsLevelBoundariesValid()
        && DestinationPoint.has_value();
}

bool LevelData::IsLevelBoundariesValid() const
{
    const bool isInvalid = EngineMath::FloatsNearEqual(0.0f, LevelBoundaryMin.x)
        || EngineMath::FloatsNearEqual(0.0f, LevelBoundaryMin.y) || EngineMath::FloatsNearEqual(0.0f, LevelBoundaryMax.x)
        || EngineMath::FloatsNearEqual(0.0f, LevelBoundaryMax.y);
    return !isInvalid;
}
} // namespace Game
