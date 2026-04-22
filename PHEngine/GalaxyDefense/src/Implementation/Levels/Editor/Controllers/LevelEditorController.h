#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/Event/BroadcastEvent.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Controllers/ILevelController.h"
#include "Implementation/Events/ChangeEditModeEvent.h"
#include "Implementation/Levels/Editor/BarriersHandler.h"
#include "Implementation/Levels/Editor/LevelPlacementGrid.h"
#include "Implementation/Levels/Editor/TowersHandler.h"

#include <memory>
#include <optional>
#include <stack>
#include <unordered_map>

namespace EngineCore {
class Scene;
class Actor;
class ThirdPersonCamera;
class InputComponent;
class RuntimeGeneratedLineComponent;
} // namespace EngineCore

namespace Graphics {
class IMaterial;
}

using namespace EngineCore::GUI;
using namespace Event;

namespace Game {
class PortalActor;

class LevelEditorController : public std::enable_shared_from_this<LevelEditorController>,
                              public ILevelController,
                              public ITickable,
                              public ChangeEditModeEvent,
                              public BroadcastGameThreadEvent {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    bool leftButtonPressed{false};

    BoundingBox2D<glm::vec2> mLevelAreaBoundingBox;

    std::unique_ptr<LevelPlacementGrid> mLevelPlacementGrid;

    std::shared_ptr<Actor> mRoutePlacementGridActor;

    std::shared_ptr<Actor> mTowerPlacementGridActor;

    std::shared_ptr<Actor> mTowerPlacementPickerActor;

    std::shared_ptr<Actor> mRouteNodePickerActor;

    std::weak_ptr<::EngineCore::ThirdPersonCamera> mMainSceneCamera;

    std::shared_ptr<::EngineCore::InputComponent> mInputComponent;

    eEditModeType mCurrentEditModeType{eEditModeType::IDLE};

    std::shared_ptr<Actor> mTowersActor;

    std::shared_ptr<Actor> mGhostTowerActor;

    std::shared_ptr<Actor> mGhostPortalActor;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mGhostTowerBlendColorProperty;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mGhostPortalColorProperty;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mFinalDestinationPointColorProperty;

    std::shared_ptr<::Graphics::IMaterial> mGridLineMaterialPrefab;

    TowersHandler mTowersHandler;

    BarriersHandler mBarriersHandler;

    std::shared_ptr<Actor> mDestinationPointActor;

    std::optional<glm::vec3> mDestinationPoint;

    std::stack<std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent>> mIdleRuntimeGeneratedLineComponents;

    std::vector<std::shared_ptr<PortalActor>> mSpawnPortals;

public:
    explicit LevelEditorController(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    ~LevelEditorController() override;

    void Initialize();

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void CleanUp() override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    void ProcessEvent(const ChangeEditModeEvent* sender, const ChangeEditModeEvent::EventData_t& data) override;

    void ProcessEvent(const BroadcastGameThreadEvent* sender, const BroadcastGameThreadEvent::EventData_t& data) override;

private:
    void InitializeInternalActors();

    void InitializeRoutePlacementGrid();

    void InitializeTowerPlacementGrid();

    void InitializeGhostTower();

    void InitializeGhostPortal();

    void InitializeDestinationPointActor();

    void RestoreLineComponentsPool();

    void ReAllocateLineComponents();

    bool IsTowerPositionValid(const glm::vec3 position) const;

    glm::vec3 RaycastLevelPlane(bool& raycastWasSuccessfull, const glm::ivec2& screenSpacePoint);

    void UpdateVisibility();

    void CreateSpawnPortal(const glm::vec3& position);

    bool IsSpawnPortalPositionValid(const glm::vec3& position) const;

    bool IsPortalPositionValidAgainstStationsAndBarriers(const glm::vec3& position) const;

    void UndoLastSpawnPortal();

    std::vector<glm::vec3> CollectSpawnPortalPoints() const;
};
} // namespace Game
