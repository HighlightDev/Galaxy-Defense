#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonUiFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaEngineEventsFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/IoCore/FolderManager.h"
#include "Game/LuaFunctions/LuaGameEventsFunctions.h"

using namespace EngineCore::Scripts;

namespace EngineCore {
class InputComponent;
}

namespace Game {
class LuaUiControllerExecutor : public LuaScriptExecutorBase {
public:
    LuaUiControllerExecutor(const std::string& scriptName);

    ~LuaUiControllerExecutor();

    virtual void Initialize();
};
} // namespace Game