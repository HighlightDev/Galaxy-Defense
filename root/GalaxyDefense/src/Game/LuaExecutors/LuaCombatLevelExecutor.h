#pragma once

#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"

using namespace EngineCore::Scripts;

namespace Game {
class LevelProgressController;

class LuaCombatLevelExecutor : public LuaScriptExecutorBase {

    std::shared_ptr<::Game::LevelProgressController> mLvlProgressController;

public:
    explicit LuaCombatLevelExecutor(
        const std::string& scriptName, const std::shared_ptr<::Game::LevelProgressController>& lvlProgressController);
};
} // namespace Game