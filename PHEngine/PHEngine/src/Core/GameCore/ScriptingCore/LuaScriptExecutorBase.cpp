#include "LuaScriptExecutorBase.h"
#include "Core/CommonCore/Assertion.h"

#include <functional>
#include <iostream>
#include <string>

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

        const std::any& LuaScriptExecutorBase::GetFunctorAny(const uint64_t functionHash) const
        {
            assert(mFunctors.count(functionHash));
            return mFunctors.at(functionHash);
        }

        void LuaScriptExecutorBase::PostInit(const std::weak_ptr<Scene>& scene)
        {
           mSceneWP = scene;
        }

        void LuaScriptExecutorBase::AddFunctor(const uint64_t functorNameHash, const std::any& functor)
        {
            assert(!mFunctors.count(functorNameHash));
            mFunctors[functorNameHash] = functor;
        }
    }
}