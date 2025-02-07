#include "GameLevelFactory.h"

#include "Implementation/Levels/MainMenu/MainMenuLevel.h"
#include "Implementation/Levels/Prologue/PrologueLevel.h"

namespace Game {
std::shared_ptr<Level> GameLevelFactory::CreateLevel(const std::string& levelName) const
{
    if ("MainMenuLevel" == levelName) {
        return std::make_shared<MainMenuLevel>();
    } else if ("FirstLevel" == levelName) {
        return std::make_shared<PrologueLevel>();
    }
    return nullptr;
}

} // namespace Game