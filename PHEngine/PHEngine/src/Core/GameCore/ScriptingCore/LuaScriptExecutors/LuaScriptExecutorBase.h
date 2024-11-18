#pragma once

#include "ILuaScriptExecutable.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaWrapper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

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

            bool mIsEnabled{true};

        protected:
            std::string mScriptName;

            bool mHasOnStart{false};

            bool mHasOnUpdate{false};

            std::weak_ptr<Scene> mSceneWP;

            std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

            LuaWrapper mLuaInstance;

        public:
            LuaScriptExecutorBase(const std::string &scriptName);

            std::string GetScriptName() const;

            size_t GetUId() const;

            const std::any &GetFunctorAny(const uint64_t functionHash) const;

            const LuaWrapper &GetLuaInstance() const;

            void SetScene(const std::weak_ptr<Scene> &scene);

            void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor);

            std::weak_ptr<Scene> GetScene() const;

            std::weak_ptr<LuaScriptProcessor> GetLuaScriptProcessor() const;

            void AddFunctor(const uint64_t functorNameHash, const std::any &functor);

            void RunScript() override;

            void StopScript() override;

            void OnUpdate(const float deltaTime) override;

            void RestartScript() override;

            void CleanUp() override;

            bool IsEnabled() const;

            void SetIsEnabled(const bool isEnabled);

        protected:
            void SetScript(const std::string &scriptName);
        };
    }
}