#pragma once

#include "Core/GameCore/Scene.h"
#include "Implementation/LuaExecutors/LuaUiControllerExecutor.h"

namespace Game {
class LevelProgressController;

class LuaCombatLevelUiControllerExecutor : public LuaUiControllerExecutor {
    std::shared_ptr<LevelProgressController> mLevelProgressController;

public:
    LuaCombatLevelUiControllerExecutor(
        const std::string& scriptName, const std::shared_ptr<LevelProgressController>& lvlProgressController);

    virtual ~LuaCombatLevelUiControllerExecutor();

    void Initialize() override;
};
} // namespace Game