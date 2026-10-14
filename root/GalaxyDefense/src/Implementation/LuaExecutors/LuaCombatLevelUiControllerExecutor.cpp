#include "LuaCombatLevelUiControllerExecutor.h"

#include "Implementation/LevelProgressSystem/LevelProgressController.h"
#include "Implementation/LuaFunctions/LuaGameEventsFunctions.h"
#include "Implementation/LuaFunctions/LuaLvlProgressFunctions.h"

using namespace EngineMath;
using namespace EngineCore;

namespace Game {
LuaCombatLevelUiControllerExecutor::LuaCombatLevelUiControllerExecutor(
    const std::string& scriptName, const std::shared_ptr<LevelProgressController>& lvlProgressController)
    : LuaUiControllerExecutor(scriptName)
    , mLevelProgressController(lvlProgressController)
{
}

LuaCombatLevelUiControllerExecutor::~LuaCombatLevelUiControllerExecutor()
{
}

void LuaCombatLevelUiControllerExecutor::Initialize()
{
    mLuaFunctions
        = {std::make_shared<LuaCommonUiFunctions>(this),
           std::make_shared<LuaCommonEngineFunctions>(this),
           std::make_shared<LuaEngineEventsFunctions>(this),
           std::make_shared<LuaGameEventsFunctions>(this),
           std::make_shared<LuaLvlProgressFunctions>(this, mLevelProgressController)};

    for (const auto& luaFunction : mLuaFunctions) {
        luaFunction->Initialize();
    }
}
} // namespace Game