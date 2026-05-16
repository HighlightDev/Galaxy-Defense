#include "LuaScriptExecutorBase.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/EngineConstants.h"
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
    ext_assert(mScriptName == "", "LuaScriptExecutorBase::SetScript: Script name is already set");
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
    ext_assert(mFunctors.count(functionHash), "LuaScriptExecutorBase::GetFunctorAny: Functor not found for given function hash");
    return mFunctors.at(functionHash);
}

const LuaWrapper& LuaScriptExecutorBase::GetLuaInstance() const
{
    ext_assert(
        mLuaInstance.IsLuaScriptOpened() && mLuaInstance.GetState(),
        "LuaScriptExecutorBase::GetLuaInstance: Lua instance is not properly initialized");
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
    ext_assert(mScriptName != "", "LuaScriptExecutorBase::RunScript: Script name is not set");
    const auto& folderManager = FolderManager::GetInstance();
    const bool bScriptExecuted = mLuaInstance.ExecuteScript(folderManager->GetAbsolutePath(mScriptName));
    ext_assert(bScriptExecuted, "LuaScriptExecutorBase::RunScript: Script execution failed");

    mHasOnStart = GetLuaGlobalVariable::Value<int64_t>(mLuaInstance, "HasOnStart", -1);
    mHasOnUpdate = GetLuaGlobalVariable::Value<int64_t>(mLuaInstance, "HasOnUpdate", -1);

    if (mHasOnStart) {
        LuaFunctionInvoker<void(void*)>::Invoke(mLuaInstance, "System_OnStart", (void*)this);
    }
}

void LuaScriptExecutorBase::StopScript()
{
    mLuaInstance.StopExecution();
}

void LuaScriptExecutorBase::OnUpdate(const float deltaTimeSec)
{
    if (mHasOnUpdate) {
        ext_assert(
            ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_luaThreadName),
            "LuaScriptExecutorBase::OnUpdate: Current thread is not Lua thread");
        LuaFunctionInvoker<void(void*, float)>::Invoke(mLuaInstance, "System_OnUpdate", (void*)this, deltaTimeSec);
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