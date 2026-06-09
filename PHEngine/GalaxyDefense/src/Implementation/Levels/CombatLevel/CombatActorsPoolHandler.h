#pragma once

#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/GameObjectsCollisionType.h"
#include "Implementation/GameObjectsType.h"
#include "Implementation/LootCategoryType.h"
#include "Implementation/MissileType.h"

#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace EngineCore {
class Scene;
class Actor;
} // namespace EngineCore

namespace EnginePhysics {
class PhysicsComponent;
}

namespace Game {
class IMissileFactory;
class ElectroRayChainActor;
class MissileActor;
class SpaceObjectActor;
class SpaceshipActor;
class BarrierActor;
class PortalActor;
class LootActor;

class CombatActorsPoolHandler : public std::enable_shared_from_this<CombatActorsPoolHandler> {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::vector<std::shared_ptr<SpaceStationActor>> mSpaceStations;

    mutable std::vector<std::shared_ptr<SpaceshipActor>> mEnemySpaceships;

    std::vector<std::shared_ptr<MissileActor>> mMissilesPool;

    std::vector<std::shared_ptr<SpaceObjectActor>> mSpaceObjectsPool;

    std::vector<std::shared_ptr<ElectroRayChainActor>> mElectroRayChainActorPool;

    std::vector<std::shared_ptr<BarrierActor>> mBarriersPool;

    std::vector<std::shared_ptr<PortalActor>> mSpawnPortals;

    mutable std::unordered_map<int32_t, eGameObjectsType> mActorTypeCache;

    std::vector<std::shared_ptr<LootActor>> mLootActors;

    mutable std::function<void(const std::shared_ptr<SpaceshipActor>&)> mOnSpaceshipSpawnedCallback;

public:
    explicit CombatActorsPoolHandler(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    void CleanUp();

    std::shared_ptr<SpaceStationActor> CreateSpaceStationActor(
        const std::string& towerName, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

    std::shared_ptr<ElectroRayChainActor> GetFreeElectroChainActor();

    std::shared_ptr<MissileActor> GetFreeMissile(const eMissileType missileType);

    std::shared_ptr<MissileActor> GetMissileOwnerActorById(const int32_t actorId) const;

    std::shared_ptr<SpaceshipActor> GetFreeSpaceshipActor(const eSpaceshipType spaceshipType) const;

    std::shared_ptr<BarrierActor> GetFreeBarrierActor() const;

    std::shared_ptr<PortalActor> GetFreePortalActor() const;

    std::shared_ptr<SpaceStationActor> GetFreeSpaceStationActor() const;

    std::shared_ptr<SpaceshipActor> GetEnemyShipOwnerActorById(const int32_t actorId) const;

    std::shared_ptr<SpaceObjectActor> GetSpaceObjectOwnerActorById(const int32_t actorId) const;

    std::shared_ptr<SpaceStationActor> GetSpaceStationOwnerActorById(const int32_t actorId) const;

    std::shared_ptr<BarrierActor> GetBarrierOwnerActorById(const int32_t actorId) const;

    std::shared_ptr<LootActor> GetLootOwnerByActorId(const int32_t actorId) const;

    const std::vector<std::shared_ptr<SpaceshipActor>>& GetEnemySpaceshipActors() const;

    const std::vector<std::shared_ptr<MissileActor>>& GetMissileActors() const;

    const std::vector<std::shared_ptr<SpaceStationActor>>& GetSpaceStationActors() const;

    const std::vector<std::shared_ptr<PortalActor>> GetPortalActors() const;

    const std::vector<std::shared_ptr<BarrierActor>>& GetBarrierActors() const;

    eGameObjectsType GetGameObjectTypeByActorId(const int32_t actorId) const;

    eGameObjectsCollisionType
    GetGameObjectsCollisionType(const eGameObjectsType firstObject, const eGameObjectsType secondObject) const;

    std::shared_ptr<LootActor> GetFreeLootActor(const eLootCategory lootCategory);

    void SpawnEnemySpaceships(const int32_t count, const eSpaceshipType spaceshipType);

    void SetOnSpaceshipSpawnedCallback(std::function<void(const std::shared_ptr<SpaceshipActor>&)> callback);

    void SpawnMissiles(const eMissileType missileType, const int32_t count);

    void SpawnAsteroids(const int32_t count);

    void SpawnBarriers(const int32_t barriersCount, const int32_t pillarsCount);

    void SpawnPortals(const int32_t count, const float portalSize);

    void SpawnLoot(const int32_t count, const eLootCategory lootCategory);

    int32_t GetSpaceStationsCount() const;

    std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetSpaceStationsPhysicsComponents() const;

    std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetSpaceShipsPhysicsComponents() const;

    std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>>
    GetMissilePhysicsComponents(const std::unordered_set<eMissileType>& missileTypes) const;

    std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetBarriersPhysicsComponents() const;

    std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetLootPhysicsComponents() const;

    int32_t GetSpaceStationsCountWithState(const eSpaceStationActivityState state) const;

    std::vector<std::shared_ptr<Actor>> GetActorsByGameObjectType(const eGameObjectsType gameObjectType) const;

    std::vector<std::shared_ptr<PhysicsComponent>>
    GetPhysicsComponentsByGameObjectType(const eGameObjectsType gameObjectType) const;

    std::vector<std::shared_ptr<PhysicsComponent>>
    GetPhysicsComponentsByGameObjectTypes(const std::unordered_set<eGameObjectsType>& gameObjectTypes) const;

    std::shared_ptr<IHighlightable> GetHighlightableByType(const int32_t goId, const eGameObjectsType goType) const;

private:
    std::unique_ptr<IMissileFactory> GetMissileFactoryByType(const eMissileType missileType) const;

    std::shared_ptr<ElectroRayChainActor> SpawnElectroRayChainActor();

    std::shared_ptr<SpaceshipActor> SpawnSpaceshipActor(const eSpaceshipType spaceshipType) const;
};
} // namespace Game
