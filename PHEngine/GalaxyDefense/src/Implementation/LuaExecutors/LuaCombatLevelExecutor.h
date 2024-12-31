#pragma once

#include "Core/GameCore/ScriptingCore/LuaFunctions/ILuaFunctionable.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"

using namespace EngineCore::Scripts;

namespace Game
{
    class LevelProgressController;

    class LuaCombatLevelExecutor
        : public LuaScriptExecutorBase
    {
        std::vector<std::shared_ptr<ILuaFunctionable>> mLuaFunctions;

        std::shared_ptr<::Game::LevelProgressController> mLvlProgressController;

    public:
        explicit LuaCombatLevelExecutor(const std::string &scriptName,
                                        const std::shared_ptr<::Game::LevelProgressController> &lvlProgressController);

        void RunScript() override;

        void StopScript() override;

        void RegisterCallbacks() override;
    };
}