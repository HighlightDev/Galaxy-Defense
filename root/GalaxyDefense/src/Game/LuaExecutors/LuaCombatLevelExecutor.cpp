#include "LuaCombatLevelExecutor.h"

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaEngineObjectsCreatorFunctions.h"
#include "Core/IoCore/FolderManager.h"
#include "Game/LevelProgressSystem/LevelProgressController.h"
#include "Game/LuaFunctions/LuaLvlProgressFunctions.h"

namespace Game {
LuaCombatLevelExecutor::LuaCombatLevelExecutor(
    const std::string& scriptName, const std::shared_ptr<LevelProgressController>& lvlProgressController)
    : LuaScriptExecutorBase(scriptName)
    , mLvlProgressController(lvlProgressController)
{
    SetLuaFunctions(
        {std::make_shared<LuaCommonEngineFunctions>(this),
         std::make_shared<LuaEngineObjectsCreatorFunctions>(this),
         std::make_shared<LuaLvlProgressFunctions>(this, lvlProgressController)});
}
} // namespace Game