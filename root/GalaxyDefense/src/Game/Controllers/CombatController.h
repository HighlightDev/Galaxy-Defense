#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"
#include "Core/GameCore/ITickable.h"
#include "Game/Actors/BackgroundSpaceObjectActor.h"
#include "Game/Actors/MissileActor.h"
#include "Game/Actors/SpaceObjectActor.h"
#include "Game/Actors/SpaceStationActor.h"
#include "Game/Actors/SpaceshipActor.h"
#include "Game/Controllers/LootController.h"
#include "Game/Controllers/NavigationController.h"
#include "Game/Controllers/UserInteractionController.h"
#include "Game/Events/ChangeGameModeEvent.h"
#include "Game/Events/ElectroRaySphereContactCollisionEvent.h"
#include "Game/Events/ShootRayCollisionEvent.h"
#include "Game/GameModeTypeEnum.h"
#include "Game/GameObjectsCollisionType.h"
#include "Game/GameObjectsType.h"
#include "Game/Levels/CombatLevel/CombatActorsPoolHandler.h"
#include "Game/Levels/LevelData.h"
#include "Game/MissileType.h"
#include "ILevelController.h"

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
                         public ShootRayCollisionEvent,
                         public ElectroRaySphereContactCollisionEvent,
                         public ChangeGameModeEvent,
                         public std::enable_shared_from_this<CombatController> {
    std::weak_ptr<Scene> mScene;

    std::shared_ptr<NavigationController> mNavigationController;

    std::shared_ptr<UserInteractionController> mUserInteractionController;

    std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

    std::shared_ptr<LootController> mLootController;

    BoundingBox3D mLevelBounds;

    eGameModeType mGameModeType{eGameModeType::INIT};

public:
    CombatController(const std::weak_ptr<Scene>& scene);

    ~CombatController();

    void OnPreLevelInit() override;

    void OnLevelInit() override;

    void OnPostLevelInit() override;

    void PostPlayLevelFinished() override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override { };

    void CleanUp() override;

    void InitFromLevelData(const LevelData& levelData);

    std::shared_ptr<NavigationController> GetNavigationController() const;

protected:
    void ProcessEvent(
        const PhysicsCollisionGameThreadEvent* sender,
        const typename PhysicsCollisionGameThreadEvent::EventData_t& data) override;

    void ProcessEvent(const ShootRayCollisionEvent* sender, const typename ShootRayCollisionEvent::EventData_t& data) override;

    void ProcessEvent(
        const ElectroRaySphereContactCollisionEvent* sender,
        const typename ElectroRaySphereContactCollisionEvent::EventData_t& data) override;

    void ProcessEvent(const ChangeGameModeEvent* sender, const typename ChangeGameModeEvent::EventData_t& data) override;

private:
    void ProcessAiAction();

    void LaunchMissile(
        const std::shared_ptr<SpaceStationActor>& missileOwner,
        const glm::vec3& missileStartPosition,
        const glm::vec3& missileDirection,
        const eMissileType missileType);

    void LaunchEnemyMissile(const glm::vec3& missileStartPosition, const glm::vec3& missileDirection);

    void ValidatePoolObjects();

    void OnCombatPreparationCompleted();

    void SpawnTowerMissiles(const int32_t towersCount);
};
} // namespace Game