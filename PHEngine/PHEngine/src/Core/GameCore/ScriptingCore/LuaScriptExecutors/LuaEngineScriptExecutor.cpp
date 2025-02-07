#include "LuaEngineScriptExecutor.h"

namespace EngineCore {
namespace Scripts {
LuaEngineScriptExecutor::LuaEngineScriptExecutor(const std::string& scriptName)
    : LuaScriptExecutorBase(scriptName)
{
    mLuaFunctions = {std::make_shared<LuaCommonEngineFunctions>(this), std::make_shared<LuaEngineObjectsCreatorFunctions>(this)};
}

void LuaEngineScriptExecutor::RunScript()
{
    LuaScriptExecutorBase::RunScript();

    for (const auto& luaFunction : mLuaFunctions) {
        luaFunction->OnScriptStarted(mLuaInstance);
    }
}

void LuaEngineScriptExecutor::StopScript()
{
    LuaScriptExecutorBase::StopScript();

    for (const auto& luaFunction : mLuaFunctions) {
        luaFunction->OnScriptStopped(mLuaInstance);
    }
}

void LuaEngineScriptExecutor::RegisterCallbacks()
{
    for (const auto& luaFunction : mLuaFunctions) {
        luaFunction->SetScene(GetScene());
        luaFunction->SetLuaScriptProcessor(GetLuaScriptProcessor());
        luaFunction->RegisterCallbacks(mLuaInstance);
    }
}
} // namespace Scripts
} // namespace EngineCore