#pragma once

#include "LevelPlacementGrid.h"
#include "Implementation/Controllers/ILevelController.h"
#include "Implementation/Events/ChangeEditModeEvent.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Event/BroadcastEvent.h"

#include <memory>
#include <stack>

namespace EngineCore
{
    class Scene;
    class Actor;
    class ThirdPersonCamera;
    class InputComponent;
    class RuntimeGeneratedQuadraticBezierCurveComponent;
}

using namespace EngineCore::GUI;
using namespace Event;

namespace Game
{
    class LevelEditorController
        : public std::enable_shared_from_this<LevelEditorController>,
          public ILevelController,
          public ITickable,
          public ChangeEditModeEvent,
          public BroadcastGameThreadEvent
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        BoundingBox2D<glm::vec2> mLevelAreaBoundingBox;

        std::unique_ptr<LevelPlacementGrid> mLevelPlacementGrid;

        std::shared_ptr<Actor> mRoutePlacementGridActor;

        std::shared_ptr<Actor> mTowerPlacementGridActor;

        std::shared_ptr<Actor> mTowerPlacementPickerActor;

        std::shared_ptr<Actor> mRouteNodePickerActor;

        std::weak_ptr<::EngineCore::ThirdPersonCamera> mMainSceneCamera;

        std::shared_ptr<::EngineCore::InputComponent> mInputComponent;

        eEditModeType mCurrentEditModeType{eEditModeType::IDLE};

        std::shared_ptr<Actor> mBezierCurvesActor;

        // todo: rework
        std::stack<std::shared_ptr<RuntimeGeneratedQuadraticBezierCurveComponent>> mActiveCurveComponents;

        std::stack<std::shared_ptr<RuntimeGeneratedQuadraticBezierCurveComponent>> mIdleCurveComponents;

        bool leftButtonPressed = false;

        std::vector<glm::vec3> bezierControlPointsList;

        void SpawnBezierCurveComponent(const int32_t bezierIndex, const glm::vec3 &pointA, const glm::vec3 &controlPoint, const glm::vec3 &pointB);

    public:
        explicit LevelEditorController(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

        ~LevelEditorController() override;

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void CleanUp() override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void ProcessEvent(const ChangeEditModeEvent::EventData_t &data) override;

        void ProcessEvent(const BroadcastGameThreadEvent::EventData_t &data) override;

        void SetLevelAreaBoundingBox(const BoundingBox2D<glm::vec2> &levelAreaBoundingBox);

    private:
        void Initialize();

        void InitializeRoutePlacementGrid();

        void InitializeTowerPlacementGrid();
    };
}
