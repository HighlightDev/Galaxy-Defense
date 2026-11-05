#include "LuaUiControllerExecutor.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"

using namespace EngineMath;
using namespace EngineCore;

namespace Game {
LuaUiControllerExecutor::LuaUiControllerExecutor(const std::string& scriptName)
    : LuaScriptExecutorBase(scriptName)
{
}

void LuaUiControllerExecutor::Initialize()
{
    SetLuaFunctions(
        {std::make_shared<LuaCommonUiFunctions>(this),
         std::make_shared<LuaCommonEngineFunctions>(this),
         std::make_shared<LuaEngineEventsFunctions>(this),
         std::make_shared<LuaGameEventsFunctions>(this)});

    for (const auto& luaFunction : mLuaFunctions) {
        luaFunction->Initialize();
    }
}

LuaUiControllerExecutor::~LuaUiControllerExecutor()
{
}
} // namespace Game