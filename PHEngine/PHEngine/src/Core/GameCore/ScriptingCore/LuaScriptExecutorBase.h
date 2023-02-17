#pragma once

#include "ILuaScriptExecutable.h"
#include "LuaWrapper.h"
#include "Core/GameCore/Scene.h"

#include <unordered_map>
#include <any>
#include <tuple>

using namespace EngineCore;

namespace EngineCore
{
    namespace Scripts
    {
        class LuaScriptExecutorBase : public ILuaScriptExecutable
        {
            static size_t sUid;

            size_t mUId;

            std::unordered_map<uint64_t, std::any> mFunctors;

        protected:
            std::string mScriptName;

            bool mHasOnStart{false};

            bool mHasOnUpdate{false};

            std::weak_ptr<Scene> mSceneWP;

            LuaWrapper mLuaInstance;

        public:
            LuaScriptExecutorBase();

            std::string GetScriptName() const;

            size_t GetUId() const;

            const std::any &GetFunctorAny(const uint64_t functionHash) const;

            void SetScene(const std::weak_ptr<Scene> &scene);

            void AddFunctor(const uint64_t functorNameHash, const std::any &functor);

            void RunScript() override;

        protected:
            void SetScript(const std::string &scriptName);
        };
    }
}