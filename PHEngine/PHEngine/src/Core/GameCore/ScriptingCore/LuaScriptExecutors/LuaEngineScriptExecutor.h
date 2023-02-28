#pragma once
#include "LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaEngineObjectsCreatorFunctions.h"

#include <memory>

namespace EngineCore
{
    namespace Scripts
    {
        class LuaEngineScriptExecutor
            : public LuaScriptExecutorBase
        {
            std::unique_ptr<LuaCommonEngineFunctions> mLuaCommonEngineFunctions;
            std::unique_ptr<LuaEngineObjectsCreatorFunctions> mLuaEngineObjectsCreatorFunctions;

        public:
            explicit LuaEngineScriptExecutor(const std::string &scriptName);

            void RunScript() override;

            void StopScript() override;

            void RegisterCallbacks() override;
        };
    }
}