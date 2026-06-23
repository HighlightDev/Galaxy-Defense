#pragma once

#include "Core/IoCore/FileWatcher.h"
#include "Implementation/Levels/Editor/Controllers/LevelEditorController.h"
#include "Implementation/Levels/Editor/Controllers/LevelEditorUiController.h"
#include "Implementation/Levels/LevelBase.h"

using namespace EngineCore;

namespace Game {
class EditorLevel : public LevelBase {
    using Base = LevelBase;

    std::unique_ptr<LevelEditorUiController> mUiController;

    std::shared_ptr<LevelEditorController> mLevelEditorController;

#ifdef DEBUG
    std::shared_ptr<FileWatcher> mFileWatcher;
#endif

public:
    EditorLevel();

    ~EditorLevel() override;

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
};

} // namespace Game
