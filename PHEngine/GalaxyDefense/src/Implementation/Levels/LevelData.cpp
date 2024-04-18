#include "LevelData.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Game
{
    bool LevelData::isDataValid() const
    {
        return LevelName != "" &&
               TowersData.size() &&
               RoutesData.size() &&
               IsLevelBoundariesValid();
    }

    bool LevelData::IsLevelBoundariesValid() const
    {
        const bool isInvalid = EngineMath::FloatsNearEqual(0.0f, LevelBoundaryExtent.x) ||
                               EngineMath::FloatsNearEqual(0.0f, LevelBoundaryExtent.y) ||
                               EngineMath::FloatsNearEqual(0.0f, LevelBoundaryOrigin.x) ||
                               EngineMath::FloatsNearEqual(0.0f, LevelBoundaryOrigin.y);
        return !isInvalid;
    }
}
