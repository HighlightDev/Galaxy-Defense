#include "LuaCombatLevelExecutor.h"

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaEngineObjectsCreatorFunctions.h"
#include "Core/IoCore/FolderManager.h"

#include "Implementation/LuaFunctions/LuaLvlProgressFunctions.h"
#include "Implementation/LevelProgressSystem/LevelProgressController.h"

namespace Game
{
    LuaCombatLevelExecutor::LuaCombatLevelExecutor(const std::string &scriptName,
                                                   const std::shared_ptr<LevelProgressController> &lvlProgressController)
        : LuaScriptExecutorBase(scriptName),
          mLuaFunctions({std::make_shared<LuaCommonEngineFunctions>(this),
                         std::make_shared<LuaEngineObjectsCreatorFunctions>(this),
                         std::make_shared<LuaLvlProgressFunctions>(this, lvlProgressController)}),
          mLvlProgressController(lvlProgressController)
    {
    }

    void LuaCombatLevelExecutor::RunScript()
    {
        LuaScriptExecutorBase::RunScript();

        for (const auto &luaFunction : mLuaFunctions)
        {
            luaFunction->OnScriptStarted(mLuaInstance);
        }
    }

    void LuaCombatLevelExecutor::StopScript()
    {
        LuaScriptExecutorBase::StopScript();

        for (const auto &luaFunction : mLuaFunctions)
        {
            luaFunction->OnScriptStopped(mLuaInstance);
        }
    }

    void LuaCombatLevelExecutor::RegisterCallbacks()
    {
        for (const auto &luaFunction : mLuaFunctions)
        {
            luaFunction->SetScene(GetScene());
            luaFunction->SetLuaScriptProcessor(GetLuaScriptProcessor());
            luaFunction->RegisterCallbacks(mLuaInstance);
        }
    }
}