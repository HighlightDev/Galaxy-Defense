#pragma once

#include "Implementation/Levels/LevelBase.h"
#include "Implementation/Levels/Prologue/Controllers/UiController.h"

using namespace EngineCore;

namespace Game {
class CombatController;

class PrologueLevel : public LevelBase {
    using Base = LevelBase;

    std::shared_ptr<CombatController> mCombatController;

    std::unique_ptr<UiController> mUiController;

public:
    PrologueLevel();

    ~PrologueLevel() override;

    void InitLevel() override;

    void PreLevelInit() override;

    void PostLevelInit() override;

    void PostPlayLevelFinished() override;

    void RunLuaBuildLevelScript();

    void UnloadLevel() override;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override;

private:
    void CreateScene();
};

} // namespace Game
