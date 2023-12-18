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
            std::vector<std::shared_ptr<ILuaFunctionable>> mLuaFunctions;

        public:
            explicit LuaEngineScriptExecutor(const std::string &scriptName);

            void RunScript() override;

            void StopScript() override;

            void RegisterCallbacks() override;
        };
    }
}