#include "LuaUiControllerExecutor.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"

using namespace EngineMath;
using namespace EngineCore;

namespace TestFeatures {
LuaUiControllerExecutor::LuaUiControllerExecutor(const std::string& scriptName)
    : LuaScriptExecutorBase(scriptName)
{
    SetLuaFunctions(
        {std::make_shared<LuaCommonUiFunctions>(this),
         std::make_shared<LuaCommonEngineFunctions>(this),
         std::make_shared<LuaEngineEventsFunctions>(this)});
}

LuaUiControllerExecutor::~LuaUiControllerExecutor()
{
}
} // namespace TestFeatures