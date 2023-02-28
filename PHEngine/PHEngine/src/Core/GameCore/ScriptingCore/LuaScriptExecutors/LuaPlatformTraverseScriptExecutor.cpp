#include "LuaPlatformTraverseScriptExecutor.h"

namespace EngineCore
{
    namespace Scripts
    {
        LuaPlatformTraverseScriptExecutor::LuaPlatformTraverseScriptExecutor(const std::string &scriptName,
                                                                             ::EngineCore::PlatformTraverseComponent *owner)
            : LuaScriptExecutorBase(scriptName),
              mPlatformTraverseComponentFunctions(std::make_unique<LuaPlatformTraverseComponentFunctions>(owner, this))
        {
        }

        void LuaPlatformTraverseScriptExecutor::RunScript()
        {
            LuaScriptExecutorBase::RunScript();

            mPlatformTraverseComponentFunctions->OnScriptStarted(mLuaInstance);
        }

        void LuaPlatformTraverseScriptExecutor::StopScript()
        {
            LuaScriptExecutorBase::StopScript();

            mPlatformTraverseComponentFunctions->OnScriptStopped(mLuaInstance);
        }

        void LuaPlatformTraverseScriptExecutor::RegisterCallbacks()
        {
            mPlatformTraverseComponentFunctions->SetScene(GetScene());
            mPlatformTraverseComponentFunctions->SetLuaScriptProcessor(GetLuaScriptProcessor());

            mPlatformTraverseComponentFunctions->RegisterCallbacks(mLuaInstance);
        }
    }
}