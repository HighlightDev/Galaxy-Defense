#include "CombatActorsPoolHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/Actors/BlackHoleMissileActor.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Factories/AsteroidFactory.h"
#include "Implementation/Factories/BarrierFactory.h"
#include "Implementation/Factories/BlackHoleMissileFactory.h"
#include "Implementation/Factories/BombMissileFactory.h"
#include "Implementation/Factories/ElectroRayChainFactory.h"
#include "Implementation/Factories/ElectroRayFactory.h"
#include "Implementation/Factories/FreezingMissileFactory.h"
#include "Implementation/Factories/FreezingRayFactory.h"
#include "Implementation/Factories/SpaceStationFactory.h"
#include "Implementation/Factories/SpawnPortalFactory.h"
#include "Implementation/Factories/WeakSpaceShipFactory.h"

using namespace EnginePhysics;

namespace Game {
CombatActorsPoolHandler::CombatActorsPoolHandler(const std::weak_ptr<Scene>& sceneWp)
    : mSceneWp(sceneWp)
{
}

void CombatActorsPoolHandler::CleanUp()
{
    mSpaceStations.clear();
    mEnemySpaceships.clear();
    mMissilesPool.clear();
    mSpaceObjectsPool.clear();
    mElectroRayChainActorPool.clear();
    mBarriersPool.clear();
    mSpawnPortals.clear();
}

std::shared_ptr<ElectroRayChainActor> CombatActorsPoolHandler::GetFreeElectroChainActor()
{
    const auto freeChainActorIt
        = std::find_if(mElectroRayChainActorPool.cbegin(), mElectroRayChainActorPool.cend(), [](const auto& actorInPool) {
              return eMissileActivityState::IDLE == actorInPool->GetMissileActivityState();
          });
    if (freeChainActorIt != mElectroRayChainActorPool.cend()) {
        return (*freeChainActorIt);
    }

    return SpawnElectroRayChainActor();
}

std::shared_ptr<SpaceStationActor> CombatActorsPoolHandler::CreateSpaceStationActor(
    const std::string& towerName, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const SpaceStationFactory spaceStationFactory;
    return mSpaceStations.emplace_back(spaceStationFactory.CreateSpaceStation(sceneSp, towerName, translation, rotation, scale));
}

std::shared_ptr<ElectroRayChainActor> CombatActorsPoolHandler::SpawnElectroRayChainActor()
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);

    ElectroRayChainFactory factory;
    const auto& spawnedActor = mElectroRayChainActorPool.emplace_back(std::static_pointer_cast<ElectroRayChainActor>(
        factory.CreateMissile(sceneSp, shared_from_this(), glm::vec3(), glm::vec3(), glm::vec3(1), 0.0f)));
    spawnedActor->TriggerDisabled();
    return spawnedActor;
}

void CombatActorsPoolHandler::SpawnEnemySpaceships(const int32_t count)
{
    for (size_t i = 0; i < count; ++i) {
        SpawnSpaceshipActor()->SetIsEnabled(false);
    }
}

std::shared_ptr<SpaceshipActor> CombatActorsPoolHandler::SpawnSpaceshipActor() const
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    constexpr float c_spaceshipSize = 2.5f;
    WeakSpaceShipFactory spaceShipFactory;

    return mEnemySpaceships.emplace_back(
        spaceShipFactory.CreateSpaceShip(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(c_spaceshipSize)));
}

std::shared_ptr<SpaceshipActor> CombatActorsPoolHandler::GetFreeSpaceshipActor() const
{
    const auto freeShipIt = std::find_if(mEnemySpaceships.cbegin(), mEnemySpaceships.cend(), [](const auto& spaceship) {
        return spaceship->GetSpaceshipActivityState() == eSpaceshipActivityState::IDLE;
    });
    return freeShipIt == mEnemySpaceships.cend() ? SpawnSpaceshipActor() : *freeShipIt;
}

const std::vector<std::shared_ptr<SpaceshipActor>>& CombatActorsPoolHandler::GetEnemySpaceshipActors() const
{
    return mEnemySpaceships;
}

const std::vector<std::shared_ptr<MissileActor>>& CombatActorsPoolHandler::GetMissileActors() const
{
    return mMissilesPool;
}

const std::vector<std::shared_ptr<SpaceStationActor>>& CombatActorsPoolHandler::GetSpaceStationActors() const
{
    return mSpaceStations;
}

void CombatActorsPoolHandler::SpawnMissiles(const eMissileType missileType, const int32_t count, const float hitRadius)
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto& missileFactory = GetMissileFactoryByType(missileType);
    assert(missileFactory);

    for (int32_t i = 0; i < count; ++i) {
        const auto& missile = mMissilesPool.emplace_back(
            missileFactory->CreateMissile(sceneSp, shared_from_this(), glm::vec3(), glm::vec3(), glm::vec3(1.0f), hitRadius));
        assert(missile);
        missile->SetIsEnabled(false);
    }
}

void CombatActorsPoolHandler::SpawnAsteroids(const int32_t count)
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto& asteroidsFactory = std::make_unique<AsteroidFactory>();
    const auto& asteroid
        = mSpaceObjectsPool.emplace_back(asteroidsFactory->CreateSpaceObject(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
    asteroid->SetIsEnabled(false);
}

void CombatActorsPoolHandler::SpawnBarriers(const int32_t barriersCount, const int32_t pillarsCount)
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto& barriersFactory = std::make_unique<BarrierFactory>();
    for (int32_t i = 0; i < barriersCount; ++i) {
        const auto& barrier = mBarriersPool.emplace_back(
            barriersFactory->CreateBarrier(pillarsCount, sceneSp, glm::vec3(), glm::vec3(), glm::vec3(6.0f)));
        barrier->SetIsEnabled(false);
    }
}

void CombatActorsPoolHandler::SpawnPortals(const int32_t count, const float portalSize)
{
    const auto& sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto& portalsFactory = std::make_unique<SpawnPortalFactory>();
    for (int32_t i = 0; i < count; ++i) {
        const auto& portal = mSpawnPortals.emplace_back(
            portalsFactory->CreatePortal(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1.0f), portalSize));
        portal->SetIsEnabled(false);
    }
}

std::shared_ptr<Actor> CombatActorsPoolHandler::GetFreePortalActor() const
{
    const auto idlePortalIt
        = std::find_if(mSpawnPortals.cbegin(), mSpawnPortals.cend(), [](const auto& portalSp) { return !portalSp->IsEnabled(); });

    return idlePortalIt == mSpawnPortals.cend() ? nullptr : *idlePortalIt;
}

std::shared_ptr<BarrierActor> CombatActorsPoolHandler::GetFreeBarrierActor() const
{
    const auto idleBarrierIt = std::find_if(
        mBarriersPool.cbegin(), mBarriersPool.cend(), [](const auto& barrierSp) { return !barrierSp->IsEnabled(); });

    return idleBarrierIt == mBarriersPool.cend() ? nullptr : *idleBarrierIt;
}

int32_t CombatActorsPoolHandler::GetSpaceStationsCount() const
{
    return mSpaceStations.size();
}

std::shared_ptr<MissileActor> CombatActorsPoolHandler::GetFreeMissile(const eMissileType missileType) const
{
    const auto idleBulletIt = std::find_if(mMissilesPool.cbegin(), mMissilesPool.cend(), [missileType](const auto& missile) {
        return (eMissileActivityState::IDLE == missile->GetMissileActivityState() && missileType == missile->GetMissileType());
    });

    return idleBulletIt == mMissilesPool.cend() ? nullptr : *idleBulletIt;
}

std::shared_ptr<SpaceStationActor> CombatActorsPoolHandler::GetFreeSpaceStationActor() const
{
    const auto idleSpaceStationIt = std::find_if(mSpaceStations.cbegin(), mSpaceStations.cend(), [](const auto& spaceStationSp) {
        return eSpaceStationActivityState::IDLE == spaceStationSp->GetState();
    });

    return idleSpaceStationIt == mSpaceStations.cend() ? nullptr : *idleSpaceStationIt;
}

std::unique_ptr<IMissileFactory> CombatActorsPoolHandler::GetMissileFactoryByType(const eMissileType missileType) const
{
    switch (missileType) {
    case eMissileType::BOMB:
        return std::make_unique<BombMissileFactory>();
    case eMissileType::FREEZING:
        return std::make_unique<FreezingMissileFactory>();
    case eMissileType::ELECTRO_RAY:
        return std::make_unique<ElectroRayFactory>();
    case eMissileType::BLACK_HOLE:
        return std::make_unique<BlackHoleMissileFactory>();
    case eMissileType::FREEZING_RAY:
        return std::make_unique<FreezingRayFactory>();
    default:
        return nullptr;
    }
}

std::shared_ptr<MissileActor> CombatActorsPoolHandler::GetMissileOwnerActorById(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mMissilesPool.cbegin(), mMissilesPool.cend(), [=](const auto& missile) {
        return missile->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mMissilesPool.cend() ? (*foundIt) : nullptr;
}

std::shared_ptr<SpaceshipActor> CombatActorsPoolHandler::GetEnemyShipOwnerActorById(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mEnemySpaceships.cbegin(), mEnemySpaceships.cend(), [actorId](const auto& enemyActor) {
        return enemyActor->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mEnemySpaceships.cend() ? (*foundIt) : nullptr;
}

std::shared_ptr<SpaceObjectActor> CombatActorsPoolHandler::GetSpaceObjectOwnerActorById(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mSpaceObjectsPool.cbegin(), mSpaceObjectsPool.cend(), [=](const auto& spaceObject) {
        return spaceObject->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mSpaceObjectsPool.cend() ? (*foundIt) : nullptr;
}

std::shared_ptr<SpaceStationActor> CombatActorsPoolHandler::GetSpaceStationOwnerActorById(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mSpaceStations.cbegin(), mSpaceStations.cend(), [=](const auto& spaceStation) {
        return spaceStation->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mSpaceStations.cend() ? (*foundIt) : nullptr;
}

eGameObjectsType CombatActorsPoolHandler::GetGameObjectTypeByActorId(const int32_t actorId) const
{
    const auto result = GetEnemyShipOwnerActorById(actorId) ? eGameObjectsType::SPACESHIP
        : GetMissileOwnerActorById(actorId)                 ? eGameObjectsType::MISSILE
        : GetSpaceObjectOwnerActorById(actorId)             ? eGameObjectsType::NEUTRAL_SPACE_OBJECT
        : GetSpaceStationOwnerActorById(actorId)            ? eGameObjectsType::SPACE_STATION
                                                            : eGameObjectsType::UNDEFINED;

    assert(eGameObjectsType::UNDEFINED != result);

    return result;
}

eGameObjectsCollisionType CombatActorsPoolHandler::GetGameObjectsCollisionType(
    const eGameObjectsType firstObject, const eGameObjectsType secondObject) const
{
    if ((eGameObjectsType::SPACESHIP == firstObject && eGameObjectsType::MISSILE == secondObject)
        || (eGameObjectsType::MISSILE == firstObject && eGameObjectsType::SPACESHIP == secondObject))
        return eGameObjectsCollisionType::SPACESHIP_WITH_MISSILE;

    if ((eGameObjectsType::SPACESHIP == firstObject && eGameObjectsType::NEUTRAL_SPACE_OBJECT == secondObject)
        || (eGameObjectsType::NEUTRAL_SPACE_OBJECT == firstObject && eGameObjectsType::SPACESHIP == secondObject))
        return eGameObjectsCollisionType::SPACESHIP_WITH_NEUTRAL_SPACE_OBJECT;

    if ((eGameObjectsType::MISSILE == firstObject && eGameObjectsType::NEUTRAL_SPACE_OBJECT == secondObject)
        || (eGameObjectsType::NEUTRAL_SPACE_OBJECT == firstObject && eGameObjectsType::MISSILE == secondObject))
        return eGameObjectsCollisionType::MISSILE_WITH_NEUTRAL_SPACE_OBJECT;

    return eGameObjectsCollisionType::UNDEFINED;
}

std::vector<std::shared_ptr<PhysicsComponent>> CombatActorsPoolHandler::GetSpaceStationsPhysicsComponents() const
{
    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    physicsComponents.reserve(mSpaceStations.size());
    std::transform(
        mSpaceStations.cbegin(), mSpaceStations.cend(), std::back_inserter(physicsComponents), [](const auto& spaceStationActor) {
            assert(spaceStationActor->GetPhysicsComponent());
            return spaceStationActor->GetPhysicsComponent();
        });
    return physicsComponents;
}

std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>> CombatActorsPoolHandler::GetSpaceShipsPhysicsComponents() const
{
    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    physicsComponents.reserve(mEnemySpaceships.size());
    std::transform(
        mEnemySpaceships.cbegin(), mEnemySpaceships.cend(), std::back_inserter(physicsComponents), [](const auto& spaceship) {
            assert(spaceship->GetPhysicsComponent());
            return spaceship->GetPhysicsComponent();
        });
    return physicsComponents;
}

std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>>
CombatActorsPoolHandler::GetMissilePhysicsComponents(const eMissileType missileType) const
{
    if (eMissileType::NONE == missileType || eMissileType::ELECTRO_RAY == missileType) {
        return {};
    }

    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    for (const auto& missile : mMissilesPool) {
        if (missileType == missile->GetMissileType()) {
            if (eMissileType::BLACK_HOLE == missileType) {
                const auto& blackHoleMissile = std::static_pointer_cast<BlackHoleMissileActor>(missile);
                assert(physicsComponents.emplace_back(blackHoleMissile->GetCombatActivePhaseActor()->GetPhysicsComponent()));
                assert(physicsComponents.emplace_back(blackHoleMissile->GetExplosionPhaseActor()->GetPhysicsComponent()));
            } else {
                assert(physicsComponents.emplace_back(missile->GetPhysicsComponent()));
            }
        }
    }
    return physicsComponents;
}

int32_t CombatActorsPoolHandler::GetSpaceStationsCountWithState(const eSpaceStationActivityState state) const
{
    return std::count_if(mSpaceStations.cbegin(), mSpaceStations.cend(), [seekState = state](const auto& spaceStationSp) {
        return seekState == spaceStationSp->GetState();
    });
}
} // namespace Game
