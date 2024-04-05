#include "LevelData.h"

namespace Game
{
    bool LevelData::isDataValid() const
    {
        return LevelName != "" &&
               TowersData.size() &&
               RoutesData.size();
    }
}
