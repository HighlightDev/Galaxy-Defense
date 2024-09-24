#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/vec3.hpp>

#include "Implementation/GameObjectsType.h"
#include "Implementation/GameObjectsCollisionType.h"
#include "Implementation/MissileType.h"

namespace EngineCore
{
    class Scene;
    class Actor;
}

namespace EnginePhysics
{
    class PhysicsComponent;
}

namespace Game
{
    class ElectroRayChainActor;
    class MissileActor;
    class SpaceObjectActor;
    class SpaceStationActor;
    class SpaceshipActor;
    class IMissileFactory;

    class CombatActorsPoolHandler
        : public std::enable_shared_from_this<CombatActorsPoolHandler>
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::vector<std::shared_ptr<SpaceStationActor>> mSpaceStations;

        std::vector<std::shared_ptr<SpaceshipActor>> mEnemySpaceships;

        std::vector<std::shared_ptr<MissileActor>> mMissilesPool;

        std::vector<std::shared_ptr<SpaceObjectActor>> mSpaceObjectsPool;

        std::vector<std::shared_ptr<ElectroRayChainActor>> mElectroRayChainActorPool;

    public:
        explicit CombatActorsPoolHandler(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

        void CleanUp();

        std::shared_ptr<SpaceStationActor> CreateSpaceStationActor(const std::string &towerName,
                                                                   const glm::vec3 &translation,
                                                                   const glm::vec3 &rotation,
                                                                   const glm::vec3 &scale);

        std::shared_ptr<ElectroRayChainActor> GetFreeElectroChainActor();

        std::shared_ptr<MissileActor> GetFreeMissile(const eMissileType missileType) const;

        std::shared_ptr<MissileActor> GetMissileOwnerActorById(const int32_t actorId) const;

        std::shared_ptr<SpaceshipActor> GetEnemyShipOwnerActorById(const int32_t actorId) const;

        std::shared_ptr<SpaceshipActor> GetFreeSpaceshipActor();

        std::shared_ptr<SpaceObjectActor> GetSpaceObjectOwnerActorById(const int32_t actorId) const;

        std::shared_ptr<SpaceStationActor> GetSpaceStationOwnerActorById(const int32_t actorId) const;

        const std::vector<std::shared_ptr<SpaceshipActor>> &GetEnemySpaceshipActors() const;

        const std::vector<std::shared_ptr<MissileActor>> &GetMissileActors() const;

        const std::vector<std::shared_ptr<SpaceStationActor>> &GetSpaceStationActors() const;

        eGameObjectsType GetGameObjectTypeByActorId(const int32_t actorId) const;

        eGameObjectsCollisionType GetGameObjectsCollisionType(const eGameObjectsType firstObject, const eGameObjectsType secondObject) const;

        void SpawnEnemySpaceships(const int32_t count);

        void SpawnMissiles(const eMissileType missileType, const int32_t count);

        void SpawnAsteroids(const int32_t count);

        int32_t GetSpaceStationsCount() const;

        std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetSpaceStationsPhysicsComponents() const;

        std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetSpaceShipsPhysicsComponents() const;

        std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> GetMissilePhysicsComponents(const eMissileType missileType) const;

    private:
        std::unique_ptr<IMissileFactory> GetMissileFactoryByType(const eMissileType missileType) const;

        std::shared_ptr<ElectroRayChainActor> SpawnElectroRayChainActor();

        std::shared_ptr<SpaceshipActor> SpawnSpaceshipActor();
    };
}
