#pragma once

#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Controllers/ILevelController.h"
#include "Implementation/LuaExecutors/LuaUiControllerExecutor.h"

#include <memory>

namespace EngineCore {
class Scene;
}

using namespace EngineCore::GUI;

namespace Game {
class LevelEditorUiController : public ITickable, public ILevelController {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::shared_ptr<OverlayManager> mOverlayManager;

    size_t mExecutorId;

public:
    LevelEditorUiController(const std::weak_ptr<::EngineCore::Scene>& scene);

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void CleanUp() override;

    void RestartLuaScripts();

private:
    void Initialize();
};
} // namespace Game