#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Event/BroadcastEvent.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"
#include "Core/GameCore/ITickable.h"
#include "ILevelController.h"
#include "Implementation/Actors/BackgroundSpaceObjectActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Controllers/NavigationController.h"
#include "Implementation/Controllers/UserInteractionController.h"
#include "Implementation/Events/ChangeGameModeEvent.h"
#include "Implementation/Events/ElectroRayCollisionEvent.h"
#include "Implementation/Events/ElectroRaySphereContactCollisionEvent.h"
#include "Implementation/GameModeTypeEnum.h"
#include "Implementation/GameObjectsCollisionType.h"
#include "Implementation/GameObjectsType.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Implementation/Levels/LevelData.h"
#include "Implementation/MissileType.h"

#include <memory>
#include <utility>

using namespace EngineCore;
using namespace Event;

namespace EngineCore {
class Scene;
class Actor;
} // namespace EngineCore

namespace Game {
class CombatController : public ITickable,
                         public ILevelController,
                         public PhysicsCollisionGameThreadEvent,
                         public ElectroRayCollisionEvent,
                         public ElectroRaySphereContactCollisionEvent,
                         public BroadcastGameThreadEvent,
                         public ChangeGameModeEvent,
                         public std::enable_shared_from_this<CombatController> {
    std::weak_ptr<Scene> mScene;

    std::shared_ptr<NavigationController> mNavigationController;

    std::shared_ptr<UserInteractionController> mUserInteractionController;

    std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

    BoundingBox3D mLevelBounds;

    std::unordered_map<std::string, GameThreadTimer> mSpawnEnemyOnRouteTimers;

    eGameModeType mGameModeType{eGameModeType::INIT};

public:
    CombatController(const std::weak_ptr<Scene>& scene);

    ~CombatController();

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override { };

    void CleanUp() override;

    void InitFromLevelData(const LevelData& levelData);

protected:
    void ProcessEvent(
        const PhysicsCollisionGameThreadEvent* sender,
        const typename PhysicsCollisionGameThreadEvent::EventData_t& data) override;

    void
    ProcessEvent(const ElectroRayCollisionEvent* sender, const typename ElectroRayCollisionEvent::EventData_t& data) override;

    void ProcessEvent(
        const ElectroRaySphereContactCollisionEvent* sender,
        const typename ElectroRaySphereContactCollisionEvent::EventData_t& data) override;

    void
    ProcessEvent(const BroadcastGameThreadEvent* sender, const typename BroadcastGameThreadEvent::EventData_t& data) override;

    void ProcessEvent(const ChangeGameModeEvent* sender, const typename ChangeGameModeEvent::EventData_t& data) override;

private:
    void ProcessAiAction(); // todo: move this functionality to AI controller

    void LaunchMisile(
        const std::shared_ptr<Actor>& missileOwner,
        const glm::vec3& missileStartPosition,
        const glm::vec3& missileDirection,
        const eMissileType missileType);

    void ValidatePoolObjects();

    void UpdateMissilesData();

    void OnReadyToShoot();

    void OnCombatPreparationCompleted();
};
} // namespace Game