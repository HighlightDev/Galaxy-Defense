#include "GalaxyDefensePrefabLevelFactory.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Game/Levels/CombatLevel/CombatLevel.h"
#include "Game/Levels/Editor/EditorLevel.h"
#include "Game/Levels/MainMenu/MainMenuLevel.h"

namespace Game {
std::shared_ptr<Level>
GalaxyDefensePrefabLevelFactory::CreateLevel(const std::string& levelPrefabName, const std::string& levelDataJson) const
{
    LogInfo("GalaxyDefensePrefabLevelFactory::CreateLevel: ", levelPrefabName);
    if ("MainMenuLevel" == levelPrefabName) {
        return std::make_shared<MainMenuLevel>(levelPrefabName, levelDataJson);
    } else if ("CombatLevel" == levelPrefabName) {
        return std::make_shared<CombatLevel>(levelPrefabName, levelDataJson);
    } else if ("EditorLevel" == levelPrefabName) {
        return std::make_shared<EditorLevel>(levelPrefabName, levelDataJson);
    }
    return nullptr;
}

} // namespace Game