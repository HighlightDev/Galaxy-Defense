#pragma once

#include "Implementation/Levels/CombatLevel/Controllers/CombatLevelUiController.h"
#include "Implementation/Levels/LevelBase.h"
#include "Implementation/Levels/LevelData.h"

using namespace EngineCore;

namespace Game {
class CombatController;

class CombatLevel : public LevelBase {
    using Base = LevelBase;

    std::shared_ptr<CombatController> mCombatController;

    std::unique_ptr<CombatLevelUiController> mUiController;

public:
    CombatLevel();

    ~CombatLevel() override;

    void InitLevel() override;

    void PreLevelInit() override;

    void PostLevelInit() override;

    void PostPlayLevelFinished() override;

    void RunLuaBuildLevelScript();

    void UnloadLevel() override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void RestartLuaScripts() override;

private:
    void CreateScene();

    LevelData LoadLevelDataFromFile(const std::string& levelName) const;
};

} // namespace Game
