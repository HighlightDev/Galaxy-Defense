#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Implementation/Controllers/ILevelController.h"
#include "Implementation/LuaExecutors/LuaUiControllerExecutor.h"

#include <memory>

namespace EngineCore
{
    class Scene;
    class InputComponent;
}

using namespace EngineCore::GUI;

namespace Game
{
    class LevelEditorUiController : public ITickable,
                                    public ILevelController
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::shared_ptr<OverlayManager> mOverlayManager;

        std::shared_ptr<::EngineCore::InputComponent> mInputComponent;

        size_t mExecutorId;

    public:
        LevelEditorUiController(const std::weak_ptr<::EngineCore::Scene> &scene);

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void CleanUp() override;

    private:
        void Initialize();
    };
}