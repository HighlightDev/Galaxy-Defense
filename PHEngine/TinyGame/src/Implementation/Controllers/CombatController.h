#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"
#include "Core/GameCore/ITickable.h"
#include "ILevelController.h"
#include "Implementation/Actors/BackgroundSpaceObjectActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Events/MainPlayerActionEvent.h"
#include "Implementation/Events/RayCollisionEvent.h"
#include "Implementation/Events/SphereContactCollisionEvent.h"
#include "Implementation/GameObjectsCollisionType.h"
#include "Implementation/GameObjectsType.h"
#include "Implementation/Pools/ElectroRayChainActorPool.h"

#include <memory>
#include <utility>

using namespace EngineCore;
using namespace Event;

namespace EngineCore {
class Scene;
class Actor;
class ACamera;
} // namespace EngineCore

namespace Game {
class SpaceShipPlayerController;

class CombatController : public ITickable,
                         public ILevelController,
                         public MainPlayerActionEvent,
                         public PhysicsCollisionEvent,
                         public RayCollisionEvent,
                         public SphereContactCollisionEvent,
                         public ICameraTransformChangeNotifyable,
                         public std::enable_shared_from_this<CombatController> {
    std::weak_ptr<Scene> mScene;

    std::shared_ptr<Actor> mPlayerShip;

    std::shared_ptr<SpaceShipPlayerController> mMainPlayerActorController;

    std::vector<std::shared_ptr<SpaceshipActor>> mEnemies;

    std::vector<std::shared_ptr<MissileActor>> mMissilesPool;

    std::vector<std::shared_ptr<SpaceObjectActor>> mSpaceObjectsPool;

    std::vector<std::shared_ptr<BackgroundSpaceObjectActor>> mBackgroundSpaceObjects;

    std::shared_ptr<ElectroRayChainActorPool> mElectroRayChainActorPool; // todo: rework with pointer to generic interface

    float mCoolDownTime = 0.2f;

    bool bIsCoolDownInProgress = false;

    BoundingBox3D mLevelBounds;

    std::unique_ptr<BoundingBox3D> mCameraVisibilityArea; // todo: prepare a better solution

    GameThreadTimer mBackgroundPlanetsSpawnTimer;

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

    void SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController>& mainPlayerActorController);

protected:
    void ProcessEvent(const typename MainPlayerActionEvent::EventData_t& data) override;

    void ProcessEvent(const typename PhysicsCollisionEvent::EventData_t& data) override;

    void ProcessEvent(const typename RayCollisionEvent::EventData_t& data) override;

    void ProcessEvent(const typename SphereContactCollisionEvent::EventData_t& data) override;

    void OnCameraTransformChanged(::EngineCore::ACamera* eventSrc) override;

private:
    void CreateWeaponBulletPool(const std::shared_ptr<Scene>& sceneSp);

    void CreateAsteroidsPool(const std::shared_ptr<Scene>& sceneSp);

    void CreateBackgroundSpaceObjectsPool(const std::shared_ptr<Scene>& sceneSp);

    void ShootBullet(const glm::vec3& bulletStartPosition);

    void FlushToPoolUsedBullets();

    void UpdateMissilesData();

    std::shared_ptr<MissileActor> GetMissileOwnerActorById(const int32_t actorId) const;

    std::shared_ptr<SpaceshipActor> GetEnemyShipOwnerActorById(const int32_t actorId) const;

    std::shared_ptr<SpaceObjectActor> GetSpaceObjectOwnerActorById(const int32_t actorId) const;

    eGameObjectsType GetGameObjectTypeByActorId(const int32_t actorId) const;

    eGameObjectsCollisionType
    GetGameObjectsCollisionType(const eGameObjectsType firstObject, const eGameObjectsType secondObject) const;

    glm::vec3 GenRandomPositionForSpaceship() const;
    glm::vec3 GenRandomPositionForSpaceObject() const;
    glm::vec3 GetRandomPositionForBackgroundSpaceObject() const;

    void OnBackgroundPlanetsSpawnTimerTimeout();
};
} // namespace Game