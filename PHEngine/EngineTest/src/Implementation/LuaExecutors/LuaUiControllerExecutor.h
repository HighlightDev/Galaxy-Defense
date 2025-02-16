#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonUiFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaEngineEventsFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/IoCore/FolderManager.h"

using namespace EngineCore::Scripts;

namespace EngineCore {
class InputComponent;
}

namespace TestFeatures {
class LuaUiControllerExecutor : public LuaScriptExecutorBase {
    std::unique_ptr<LuaCommonUiFunctions> mLuaCommonUiCallbacks;
    std::unique_ptr<LuaCommonEngineFunctions> mLuaCommonEngineFunctions;
    std::unique_ptr<LuaEngineEventsFunctions> mLuaEngineEventsFunctions;

public:
    LuaUiControllerExecutor(const std::string& scriptName);

    ~LuaUiControllerExecutor();

    void RunScript() override;

    void StopScript() override;

    void RegisterCallbacks() override;
};
} // namespace TestFeatures