#pragma once

#include "Core/GameCore/ScriptingCore/LuaWrapper.h"

#include <memory>

namespace EngineCore
{
    class Scene;
}

namespace EngineCore
{
    namespace Scripts
    {
        class LuaScriptExecutorBase;
        class LuaScriptProcessor;

        class ILuaFunctionable
        {
        public:
            virtual void OnScriptStarted(const LuaWrapper &luaWrapper) = 0;

            virtual void OnScriptStopped(const LuaWrapper &luaWrapper) = 0;

            virtual void RegisterCallbacks(const LuaWrapper &luaWrapper) = 0;

            virtual void SetScene(const std::weak_ptr<Scene> &sceneWp) = 0;

            virtual void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor) = 0;

            virtual void Initialize() = 0;
        };
    }
}