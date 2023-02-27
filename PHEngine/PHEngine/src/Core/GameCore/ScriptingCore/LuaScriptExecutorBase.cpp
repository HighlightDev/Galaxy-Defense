#include "LuaScriptExecutorBase.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

#include <functional>
#include <iostream>
#include <string>

using namespace IO;

namespace EngineCore
{
    namespace Scripts
    {
        size_t LuaScriptExecutorBase::sUid = 0;
        LuaScriptExecutorBase::LuaScriptExecutorBase()
            : mUId(sUid++),
              mScriptName(),
              mFunctors()
        {
        }

        void LuaScriptExecutorBase::SetScript(const std::string &scriptName)
        {
            assert(mScriptName == "");
            mScriptName = scriptName;
        }

        std::string LuaScriptExecutorBase::GetScriptName() const
        {
            return mScriptName;
        }

        size_t LuaScriptExecutorBase::GetUId() const
        {
            return mUId;
        }

        const std::any &LuaScriptExecutorBase::GetFunctorAny(const uint64_t functionHash) const
        {
            assert(mFunctors.count(functionHash));
            return mFunctors.at(functionHash);
        }

        void LuaScriptExecutorBase::SetScene(const std::weak_ptr<Scene> &scene)
        {
            mSceneWP = scene;
        }

        void LuaScriptExecutorBase::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
        {
            mLuaScriptProcessor = scriptProcessor;
        }

        void LuaScriptExecutorBase::AddFunctor(const uint64_t functorNameHash, const std::any &functor)
        {
            assert(!mFunctors.count(functorNameHash));
            mFunctors[functorNameHash] = functor;
        }

        void LuaScriptExecutorBase::RunScript()
        {
            assert(mScriptName != "");
            const auto &folderManager = FolderManager::GetInstance();
            const bool bScriptExecuted = mLuaInstance.ExecuteScript(folderManager->GetScriptPath() + mScriptName);
            assert(bScriptExecuted);

            mHasOnStart = GetLuaGlobalVariable<int64_t>::Value(mLuaInstance, "HasOnStart", -1);
            mHasOnUpdate = GetLuaGlobalVariable<int64_t>::Value(mLuaInstance, "HasOnUpdate", -1);

            if (mHasOnStart)
            {
                LuaFunctionInvoker<void(void *)>::Invoke(mLuaInstance, "System_OnStart", (void *)this);
            }
        }
    }
}