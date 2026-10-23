#include "GameLevelFactory.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Game/Levels/CombatLevel/CombatLevel.h"
#include "Game/Levels/Editor/EditorLevel.h"
#include "Game/Levels/MainMenu/MainMenuLevel.h"

namespace Game {
std::shared_ptr<Level> GameLevelFactory::CreateLevel(const std::string& levelName) const
{
    LogInfo("GameLevelFactory::CreateLevel: ", levelName);
    if ("MainMenuLevel" == levelName) {
        return std::make_shared<MainMenuLevel>();
    } else if ("CombatLevel" == levelName) {
        return std::make_shared<CombatLevel>();
    } else if ("EditorLevel" == levelName) {
        return std::make_shared<EditorLevel>();
    }
    return nullptr;
}

} // namespace Game