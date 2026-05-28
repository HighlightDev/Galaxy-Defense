#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/Event/BroadcastEvent.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Controllers/ILevelController.h"
#include "Implementation/Events/ChangeGameModeEvent.h"
#include "Implementation/GameModeTypeEnum.h"
#include "Implementation/Levels/Editor/LevelPlacementGrid.h"
#include "Implementation/MissileType.h"
#include "Implementation/StatusTypes.h"

#include <glm/vec3.hpp>

#include <functional>
#include <unordered_map>
#include <vector>

using namespace Event;
using namespace EngineCore;

namespace EngineCore {
class InputComponent;
class ThirdPersonCamera;
class SceneComponent;
class Scene;
class Actor;
} // namespace EngineCore

namespace Game {
class BarrierActor;
class CombatActorsPoolHandler;
class SmartPicker;
class SpaceStationActor;
class CombatController;

class UserInteractionController : public ILevelController,
                                  public ITickable,
                                  public ChangeGameModeEvent,
                                  public BroadcastGameThreadEvent,
                                  public std::enable_shared_from_this<UserInteractionController> {

    enum class eUserInteractionType { IDLE, TOWER_PLACE_SELECTION, TOWER_REMOVEMENT_SELECTION, BARRIER_PLACEMENT };

    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::weak_ptr<CombatController> mParentController;

    BoundingBox3D mLevelBounds;

    std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

    eGameModeType mCurrentGameModeType{eGameModeType::INIT};

    std::weak_ptr<ThirdPersonCamera> mMainSceneCamera;

    std::shared_ptr<SmartPicker> mSmartPicker;

    std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

    int32_t mSelectedSpaceStationId{-1};

    std::shared_ptr<::EngineCore::Actor> mProjectileMarkerActor;

    std::unique_ptr<LevelPlacementGrid> mLevelPlacementGrid;

    std::shared_ptr<Actor> mTowerPlacementGridActor;

    std::shared_ptr<Actor> mPlacementAllowedAreaActor;

    std::shared_ptr<Actor> mGhostTowerActor;

    std::shared_ptr<Actor> mRemoveTowerMarkerActor;

    std::shared_ptr<GameThreadTimer> mReadyToShootTimer;

    std::shared_ptr<GameThreadTimer> mReloadPlacementTower;

    std::function<void()> mShootCallback;

    float mTowerCellSize{0.0f};

    std::vector<glm::vec3> mTowerPlacementCells;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mGhostTowerBlendColorProperty;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mRemoveTowerMarkerBlendColorProperty;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mGhostBarrierBlendColorProperty;

    eUserInteractionType mInteractionType{eUserInteractionType::IDLE};

    eMissileType mTowerMissileType{eMissileType::NONE};

    std::shared_ptr<Actor> mGhostBarrierPillarActor;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mGhostBarrierPillarBlendColorProperty;

    std::shared_ptr<BarrierActor> mCurrentBarrierActor;

    std::shared_ptr<GameThreadTimer> mSelectedSpaceStationHighlightTimer;

public:
    UserInteractionController(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    ~UserInteractionController() override;

    void SetParentController(const std::weak_ptr<CombatController>& parentController);

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override
    {
    }

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void CleanUp() override;

    void ProcessEvent(const ChangeGameModeEvent* sender, const typename ChangeGameModeEvent::EventData_t& data) override;

    void ProcessEvent(const BroadcastGameThreadEvent* sender, const typename BroadcastGameThreadEvent::EventData_t& data);

    void Initialize();

    void SetLevelBounds(const BoundingBox3D& mLevelBounds);

    void SetOnShootCallback(const std::function<void()>& callback);

    void SetActorsPoolHandler(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler);

    void
    SetTowersData(const std::unordered_map<std::string, std::tuple<glm::vec3 /*position*/, glm::vec3 /*scale*/>>& towersData);

    int32_t GetSelectedSpaceStationId() const;

    void ShowMissileProjectile();

    void HideMissileProjectile();

    glm::vec3 GetProjectileMarkerPosition() const;

    std::shared_ptr<SpaceStationActor> GetSpaceStationAtPosition(const glm::vec3& position) const;

    std::shared_ptr<BarrierActor> GetBarrierAtPosition(const glm::vec3& position) const;

    void SetUserInteractionType(const eUserInteractionType interactionType);

private:
    void InitializeTowerGrid();

    void InitializePlacementAllowedArea();

    void InitializeGhostTower();

    void InitializeGhostBarrierPillar();

    void InitializeRemoveTowerMarker();

    bool IsTowerPositionValid(const glm::vec3 position) const;

    bool IsBarrierPositionValid(const glm::vec3& position) const;

    void ProcessSpaceStationPlacementStage();

    void ProcessCombatStage();

    void TriggerPlayerStatusChangedEvent(const std::string& jsonArgs);

    void TriggerSwitchToIdleInteractionMode();

    eMissileType MissileTypeFromString(const std::string& typeStr) const;

    void SetIsHighlightSpaceStation(const int32_t spaceStationId, const bool isHighlight);
};
} // namespace Game
