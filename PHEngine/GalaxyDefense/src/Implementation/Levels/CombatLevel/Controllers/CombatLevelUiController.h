#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Implementation/Controllers/ILevelController.h"

#include <memory>

namespace EngineCore
{
    class Scene;
}

using namespace EngineCore::GUI;

namespace Game
{
    class LevelProgressController;

    class CombatLevelUiController : public ITickable,
                                    public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::shared_ptr<OverlayManager> mOverlayManager;

        std::shared_ptr<LevelProgressController> mLevelProgressController;

        size_t mExecutorId;

    public:
        CombatLevelUiController(const std::weak_ptr<::EngineCore::Scene> &scene,
                                const std::shared_ptr<LevelProgressController> &lvlProgressController);

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void CleanUp() override;

        void RestartLuaScripts();

    private:
        void Initialize();
    };
}