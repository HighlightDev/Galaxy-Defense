#include "LuaEngineScriptExecutor.h"

#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaComponentHandlerFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaEngineObjectsCreatorFunctions.h"

namespace EngineCore {
namespace Scripts {
LuaEngineScriptExecutor::LuaEngineScriptExecutor(const std::string& scriptName)
    : LuaScriptExecutorBase(scriptName)
{
    SetLuaFunctions(
        {std::make_shared<LuaCommonEngineFunctions>(this),
         std::make_shared<LuaEngineObjectsCreatorFunctions>(this),
         std::make_shared<LuaComponentHandlerFunctions>(this)});
}
} // namespace Scripts
} // namespace EngineCore