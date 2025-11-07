#pragma once

#include "Core/IoCore/FileWatcher.h"
#include "Implementation/Levels/LevelBase.h"
#include "Implementation/Levels/MainMenu/Controllers/MainMenuLevelUiController.h"

#include <memory>

using namespace EngineCore;
using namespace IO;

namespace EngineCore {
class Actor;
}

namespace Game {

class MainMenuLevel : public LevelBase {
    using Base = LevelBase;

    std::unique_ptr<MainMenuLevelUiController> mUiController;

    std::shared_ptr<Actor> mAmbientMusicDummy;

    std::unique_ptr<FileWatcher> mFileWatcher;

public:
    MainMenuLevel();

    ~MainMenuLevel() override;

    void InitLevel() override;

    void PreLevelInit() override;

    void PostLevelInit() override;

    void PostPlayLevelFinished() override;

    void UnloadLevel() override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    void RestartLuaScripts() override;

private:
    void CreateScene();

    void RunLuaBuildLevelScript();
};

} // namespace Game
