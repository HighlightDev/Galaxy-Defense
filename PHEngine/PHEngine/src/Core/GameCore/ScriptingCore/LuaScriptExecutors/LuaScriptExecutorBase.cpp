#include "LuaScriptExecutorBase.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/IoCore/FolderManager.h"

#include <functional>
#include <iostream>
#include <string>

#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace IO;

namespace EngineCore {
namespace Scripts {
size_t LuaScriptExecutorBase::sUid = 0;
LuaScriptExecutorBase::LuaScriptExecutorBase(const std::string& scriptName)
    : mUId(sUid++)
    , mScriptName(scriptName)
    , mFunctors()
{
}

void LuaScriptExecutorBase::SetScript(const std::string& scriptName)
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

const std::any& LuaScriptExecutorBase::GetFunctorAny(const uint64_t functionHash) const
{
    assert(mFunctors.count(functionHash));
    return mFunctors.at(functionHash);
}

const LuaWrapper& LuaScriptExecutorBase::GetLuaInstance() const
{
    assert(mLuaInstance.IsLuaScriptOpened() && mLuaInstance.GetState());
    return mLuaInstance;
}

void LuaScriptExecutorBase::SetScene(const std::weak_ptr<Scene>& scene)
{
    mSceneWP = scene;
}

void LuaScriptExecutorBase::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

std::weak_ptr<Scene> LuaScriptExecutorBase::GetScene() const
{
    return mSceneWP;
}

std::weak_ptr<LuaScriptProcessor> LuaScriptExecutorBase::GetLuaScriptProcessor() const
{
    return mLuaScriptProcessor;
}

void LuaScriptExecutorBase::AddFunctor(const uint64_t functorNameHash, const std::any& functor)
{
    mFunctors[functorNameHash] = functor;
}

void LuaScriptExecutorBase::RunScript()
{
    assert(mScriptName != "");
    const auto& folderManager = FolderManager::GetInstance();
    const bool bScriptExecuted = mLuaInstance.ExecuteScript(folderManager->GetScriptPath() + mScriptName);
    assert(bScriptExecuted);

    mHasOnStart = GetLuaGlobalVariable<int64_t>::Value(mLuaInstance, "HasOnStart", -1);
    mHasOnUpdate = GetLuaGlobalVariable<int64_t>::Value(mLuaInstance, "HasOnUpdate", -1);

    if (mHasOnStart) {
        LuaFunctionInvoker<void(void*)>::Invoke(mLuaInstance, "System_OnStart", (void*)this);
#ifdef DEBUG
        const auto& errorMsg = mLuaInstance.GetErrorMessageAt(-1);
        if (errorMsg.size() > 1) {
            std::cout << "ERROR: Lua script execution failed:" << errorMsg << std::endl;
            LogInfo("ERROR: Lua script execution failed:", errorMsg);
            assert(false);
        }
#endif
    }
}

void LuaScriptExecutorBase::StopScript()
{
    mLuaInstance.StopExecution();
}

void LuaScriptExecutorBase::OnUpdate(const float deltaTime)
{
    if (mHasOnUpdate) {
        assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
        LuaFunctionInvoker<void(void*, float)>::Invoke(mLuaInstance, "System_OnUpdate", (void*)this, deltaTime);
#ifdef DEBUG
        const auto& errorMsg = mLuaInstance.GetErrorMessageAt(-1);
        if (errorMsg.size() > 1) {
            std::cout << "ERROR: Lua script execution failed:" << errorMsg << std::endl;
            LogInfo("ERROR: Lua script execution failed:", errorMsg);
            assert(false);
        }
#endif
    }
}

void LuaScriptExecutorBase::RestartScript()
{
    SetIsEnabled(false);
    StopScript();
    mLuaInstance.ReopenState();
    RegisterCallbacks();
    RunScript();
    SetIsEnabled(true);
}

void LuaScriptExecutorBase::CleanUp()
{
    StopScript();
}

bool LuaScriptExecutorBase::IsEnabled() const
{
    return mIsEnabled;
}

void LuaScriptExecutorBase::SetIsEnabled(const bool isEnabled)
{
    mIsEnabled = isEnabled;
}
} // namespace Scripts
} // namespace EngineCore