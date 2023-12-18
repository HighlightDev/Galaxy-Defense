#pragma once

#include "LevelPlacementGrid.h"
#include "Implementation/Controllers/ILevelController.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/ITickable.h"

#include <memory>

namespace EngineCore
{
    class Scene;
    class Actor;
    class ThirdPersonCamera;
    class InputComponent;
}

using namespace EngineCore::GUI;

namespace Game
{
    class LevelEditorController
        : public ILevelController,
          public ITickable
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        BoundingBox2D<glm::vec2> mLevelAreaBoundingBox;

        std::unique_ptr<LevelPlacementGrid> mLevelPlacementGrid;

        std::shared_ptr<Actor> mRoutePlacementGridActor;

        std::shared_ptr<Actor> mTowerPlacementPickerActor;

        std::weak_ptr<::EngineCore::ThirdPersonCamera> mMainSceneCamera;

        std::shared_ptr<::EngineCore::InputComponent> mInputComponent;

    public:
        explicit LevelEditorController(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void CleanUp() override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void SetLevelAreaBoundingBox(const BoundingBox2D<glm::vec2> &levelAreaBoundingBox);

    private:
        void Initialize();

        void InitializeRoutePlacementGrid();

        void InitializeTowerPlacementGrid();
    };
}
