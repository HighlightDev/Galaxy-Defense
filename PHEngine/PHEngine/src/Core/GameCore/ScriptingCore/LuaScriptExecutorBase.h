#pragma once

#include "ILuaScriptExecutor.h"
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
        class LuaScriptExecutorBase : public ILuaScriptExecutor
        {
            struct LuaCorePOD
            {
                bool HasOnStart;
                bool HasOnUpdate;
            };

        private:
            static size_t sUid;

            size_t mUId;

            std::unordered_map<uint64_t, std::any> mFunctors;

        protected:
            std::string mScriptName;

            LuaCorePOD mLuaCoreData;

            std::weak_ptr<Scene> mSceneWP;

            LuaWrapper mLuaInstance;

        public:
            LuaScriptExecutorBase();

            std::string GetScriptName() const override;

            size_t GetUId() const override;

            const std::any &GetFunctorAny(const uint64_t functionHash) const;

            void PostInit(const std::weak_ptr<Scene> &scene);

            void AddFunctor(const uint64_t functorNameHash, const std::any &functor);

        protected:
            void SetScript(const std::string &scriptName) override;
        };
    }
}