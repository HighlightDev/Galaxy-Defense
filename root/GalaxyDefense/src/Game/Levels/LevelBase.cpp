#include "LevelBase.h"

namespace Game {
LevelBase::LevelBase(const std::string& levelPrefabName, const std::string& levelDataJson)
    : Level(levelPrefabName, levelDataJson)
    , mLvlProgressController(std::make_shared<LevelProgressController>())
{
}
} // namespace Game
