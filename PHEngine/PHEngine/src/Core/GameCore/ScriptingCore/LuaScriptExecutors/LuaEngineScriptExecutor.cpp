#include "LuaEngineScriptExecutor.h"

namespace EngineCore
{
    namespace Scripts
    {
        LuaEngineScriptExecutor::LuaEngineScriptExecutor(const std::string &scriptName)
            : LuaScriptExecutorBase(scriptName),
              mLuaCommonEngineFunctions(std::make_unique<LuaCommonEngineFunctions>(this)),
              mLuaEngineObjectsCreatorFunctions(std::make_unique<LuaEngineObjectsCreatorFunctions>(this))
        {
        }

        void LuaEngineScriptExecutor::RunScript()
        {
            LuaScriptExecutorBase::RunScript();

            mLuaCommonEngineFunctions->OnScriptStarted(mLuaInstance);
            mLuaEngineObjectsCreatorFunctions->OnScriptStarted(mLuaInstance);
        }

        void LuaEngineScriptExecutor::StopScript()
        {
            LuaScriptExecutorBase::StopScript();

            mLuaCommonEngineFunctions->OnScriptStarted(mLuaInstance);
            mLuaEngineObjectsCreatorFunctions->OnScriptStarted(mLuaInstance);
        }

        void LuaEngineScriptExecutor::RegisterCallbacks()
        {
            mLuaCommonEngineFunctions->SetScene(GetScene());
            mLuaEngineObjectsCreatorFunctions->SetScene(GetScene());

            mLuaCommonEngineFunctions->SetLuaScriptProcessor(GetLuaScriptProcessor());
            mLuaEngineObjectsCreatorFunctions->SetLuaScriptProcessor(GetLuaScriptProcessor());

            mLuaCommonEngineFunctions->RegisterCallbacks(mLuaInstance);
            mLuaEngineObjectsCreatorFunctions->RegisterCallbacks(mLuaInstance);
        }
    }
}