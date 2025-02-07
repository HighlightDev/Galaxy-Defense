#include "LevelBase.h"

namespace Game {
LevelBase::LevelBase(const std::string& levelName)
    : Level(levelName)
    , mLvlProgressController(std::make_unique<LevelProgressController>())
{
}
} // namespace Game
