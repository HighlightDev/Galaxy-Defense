#include "CombatActorsPoolHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Scene.h"
#include "Implementation/Actors/BarrierActor.h"
#include "Implementation/Actors/GravityBombMissileActor.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Actors/LootActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/PortalActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/Factories/AsteroidFactory.h"
#include "Implementation/Factories/BarrierFactory.h"
#include "Implementation/Factories/GravityBombMissileFactory.h"
#include "Implementation/Factories/BombMissileFactory.h"
#include "Implementation/Factories/ElectroRayChainFactory.h"
#include "Implementation/Factories/ElectroRayFactory.h"
#include "Implementation/Factories/FighterSpaceShipFactory.h"
#include "Implementation/Factories/FreezingMissileFactory.h"
#include "Implementation/Factories/FreezingRayFactory.h"
#include "Implementation/Factories/LootFactory.h"
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
    mActorTypeCache.clear();
    mLootActors.clear();
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
    ext_assert(sceneSp, "Scene pointer is null in CreateSpaceStationActor");
    const SpaceStationFactory spaceStationFactory;
    return mSpaceStations.emplace_back(spaceStationFactory.CreateSpaceStation(sceneSp, towerName, translation, rotation, scale));
}

std::shared_ptr<ElectroRayChainActor> CombatActorsPoolHandler::SpawnElectroRayChainActor()
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in SpawnElectroRayChainActor");

    ElectroRayChainFactory factory;
    const auto& spawnedActor = mElectroRayChainActorPool.emplace_back(
        std::static_pointer_cast<ElectroRayChainActor>(
            factory.CreateMissile(sceneSp, shared_from_this(), glm::vec3(), glm::vec3(), glm::vec3(1))));
    spawnedActor->TriggerDisabled();
    return spawnedActor;
}

void CombatActorsPoolHandler::SpawnEnemySpaceships(const int32_t count, const eSpaceshipType spaceshipType)
{
    for (size_t i = 0; i < count; ++i) {
        const auto& newShip = SpawnSpaceshipActor(spaceshipType);
        newShip->SetIsEnabled(false);
    }
}

std::shared_ptr<SpaceshipActor> CombatActorsPoolHandler::SpawnSpaceshipActor(const eSpaceshipType spaceshipType) const
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in SpawnSpaceshipActor");

    std::unique_ptr<ISpaceShipFactory> spaceShipFactory;
    switch (spaceshipType) {
    case eSpaceshipType::FIGHTER:
        spaceShipFactory = std::make_unique<FighterSpaceShipFactory>();
        break;
    default:
        spaceShipFactory = std::make_unique<WeakSpaceShipFactory>();
        break;
    }

    const auto result = mEnemySpaceships.emplace_back(spaceShipFactory->CreateSpaceShip(
        sceneSp, glm::vec3(), glm::vec3(), glm::vec3(Game::Constants::c_spaceshipSize), Game::Constants::c_spaceshipFontSize));
    if (mOnSpaceshipSpawnedCallback) {
        mOnSpaceshipSpawnedCallback(result);
    }
    return result;
}

void CombatActorsPoolHandler::SetOnSpaceshipSpawnedCallback(std::function<void(const std::shared_ptr<SpaceshipActor>&)> callback)
{
    mOnSpaceshipSpawnedCallback = std::move(callback);
}

std::shared_ptr<SpaceshipActor> CombatActorsPoolHandler::GetFreeSpaceshipActor(const eSpaceshipType spaceshipType) const
{
    const auto freeShipIt
        = std::find_if(mEnemySpaceships.cbegin(), mEnemySpaceships.cend(), [spaceshipType](const auto& spaceship) {
              return spaceship->GetSpaceshipActivityState() == eSpaceshipActivityState::IDLE
                  && spaceship->GetSpaceshipType() == spaceshipType;
          });
    if (freeShipIt != mEnemySpaceships.cend()) {
        return *freeShipIt;
    }
    return SpawnSpaceshipActor(spaceshipType);
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

const std::vector<std::shared_ptr<PortalActor>> CombatActorsPoolHandler::GetPortalActors() const
{
    return mSpawnPortals;
}

const std::vector<std::shared_ptr<BarrierActor>>& CombatActorsPoolHandler::GetBarrierActors() const
{
    return mBarriersPool;
}

void CombatActorsPoolHandler::SpawnMissiles(const eMissileType missileType, const int32_t count)
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in SpawnMissiles");
    const auto& missileFactory = GetMissileFactoryByType(missileType);
    ext_assert(missileFactory, "Missile factory not found for missile type");

    for (int32_t i = 0; i < count; ++i) {
        const auto& missile = mMissilesPool.emplace_back(
            missileFactory->CreateMissile(sceneSp, shared_from_this(), glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
        ext_assert(missile, "Failed to create missile");
        missile->SetIsEnabled(false);
    }
}

void CombatActorsPoolHandler::SpawnAsteroids(const int32_t count)
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in SpawnAsteroids");
    const auto& asteroidsFactory = std::make_unique<AsteroidFactory>();
    const auto& asteroid
        = mSpaceObjectsPool.emplace_back(asteroidsFactory->CreateSpaceObject(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1.0f)));
    asteroid->SetIsEnabled(false);
}

void CombatActorsPoolHandler::SpawnBarriers(const int32_t barriersCount, const int32_t pillarsCount)
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in SpawnBarriers");
    const auto& barriersFactory = std::make_unique<BarrierFactory>();
    for (int32_t i = 0; i < barriersCount; ++i) {
        const auto& barrier = mBarriersPool.emplace_back(barriersFactory->CreateBarrier(
            pillarsCount, sceneSp, glm::vec3(), glm::vec3(), Game::Constants::c_barrierPillarScale));
        barrier->SetIsEnabled(false);
    }
}

void CombatActorsPoolHandler::SpawnPortals(const int32_t count, const float portalSize)
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in SpawnPortals");
    const auto& portalsFactory = std::make_unique<SpawnPortalFactory>();
    for (int32_t i = 0; i < count; ++i) {
        const auto& portal = mSpawnPortals.emplace_back(
            portalsFactory->CreatePortal(sceneSp, glm::vec3(), glm::vec3(), glm::vec3(1.0f), portalSize));
        portal->SetIsEnabled(false);
    }
}

std::shared_ptr<PortalActor> CombatActorsPoolHandler::GetFreePortalActor() const
{
    const auto idlePortalIt
        = std::find_if(mSpawnPortals.cbegin(), mSpawnPortals.cend(), [](const auto& portalSp) { return !portalSp->IsEnabled(); });

    return idlePortalIt == mSpawnPortals.cend() ? nullptr : *idlePortalIt;
}

std::shared_ptr<BarrierActor> CombatActorsPoolHandler::GetFreeBarrierActor() const
{
    const auto idleBarrierIt = std::find_if(mBarriersPool.cbegin(), mBarriersPool.cend(), [](const auto& barrierSp) {
        return eBarrierActivityState::IDLE == barrierSp->GetState();
    });

    return idleBarrierIt == mBarriersPool.cend() ? nullptr : *idleBarrierIt;
}

int32_t CombatActorsPoolHandler::GetSpaceStationsCount() const
{
    return mSpaceStations.size();
}

std::shared_ptr<MissileActor> CombatActorsPoolHandler::GetFreeMissile(const eMissileType missileType)
{
    const auto idleBulletIt = std::find_if(mMissilesPool.cbegin(), mMissilesPool.cend(), [missileType](const auto& missile) {
        return (eMissileActivityState::IDLE == missile->GetMissileActivityState() && missileType == missile->GetMissileType());
    });

    if (idleBulletIt == mMissilesPool.cend()) {
        SpawnMissiles(missileType, 1);
        return GetFreeMissile(missileType);
    } else {
        return *idleBulletIt;
    }
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
    case eMissileType::FREEZING_BOMB:
        return std::make_unique<FreezingMissileFactory>();
    case eMissileType::ELECTRO_RAY:
        return std::make_unique<ElectroRayFactory>();
    case eMissileType::BLACK_HOLE:
        return std::make_unique<GravityBombMissileFactory>();
    case eMissileType::FREEZING_RAY:
        return std::make_unique<FreezingRayFactory>();
    default:
        return nullptr;
    }
}

std::shared_ptr<MissileActor> CombatActorsPoolHandler::GetMissileOwnerActorById(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mMissilesPool.cbegin(), mMissilesPool.cend(), [actorId](const auto& missile) {
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
    const auto foundIt = std::find_if(mSpaceObjectsPool.cbegin(), mSpaceObjectsPool.cend(), [actorId](const auto& spaceObject) {
        return spaceObject->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mSpaceObjectsPool.cend() ? (*foundIt) : nullptr;
}

std::shared_ptr<SpaceStationActor> CombatActorsPoolHandler::GetSpaceStationOwnerActorById(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mSpaceStations.cbegin(), mSpaceStations.cend(), [actorId](const auto& spaceStation) {
        return spaceStation->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mSpaceStations.cend() ? (*foundIt) : nullptr;
}

std::shared_ptr<BarrierActor> CombatActorsPoolHandler::GetBarrierOwnerActorById(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mBarriersPool.cbegin(), mBarriersPool.cend(), [actorId](const auto& barrier) {
        return barrier->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mBarriersPool.cend() ? (*foundIt) : nullptr;
}

std::shared_ptr<LootActor> CombatActorsPoolHandler::GetLootOwnerByActorId(const int32_t actorId) const
{
    const auto foundIt = std::find_if(mLootActors.cbegin(), mLootActors.cend(), [actorId](const auto& loot) {
        return loot->HasEngineObjectIdInHierarchy(actorId);
    });
    return foundIt != mLootActors.cend() ? (*foundIt) : nullptr;
}

eGameObjectsType CombatActorsPoolHandler::GetGameObjectTypeByActorId(const int32_t actorId) const
{
    const auto cacheIt = mActorTypeCache.find(actorId);
    if (cacheIt != mActorTypeCache.end()) {
        return cacheIt->second;
    }

    if (const auto enemyShipSp = GetEnemyShipOwnerActorById(actorId)) {
        return mActorTypeCache.emplace(actorId, eGameObjectsType::SPACESHIP).first->second;
    } else if (const auto missileSp = GetMissileOwnerActorById(actorId)) {
        // Don't cache missiles — DamageDealerType can change when missile is recycled from pool
        return missileSp->GetDamageDealerType() == eDamageDealerType::ENEMY_SPACESHIP ? eGameObjectsType::SPACESHIP_MISSILE
                                                                                      : eGameObjectsType::TOWER_MISSILE;
    } else if (const auto spaceObjectSp = GetSpaceObjectOwnerActorById(actorId)) {
        return mActorTypeCache.emplace(actorId, eGameObjectsType::NEUTRAL_SPACE_OBJECT).first->second;
    } else if (const auto spaceStationSp = GetSpaceStationOwnerActorById(actorId)) {
        return mActorTypeCache.emplace(actorId, eGameObjectsType::SPACE_STATION).first->second;
    } else if (const auto barrierSp = GetBarrierOwnerActorById(actorId)) {
        return mActorTypeCache.emplace(actorId, eGameObjectsType::BARRIER).first->second;
    } else if (const auto lootSp = GetLootOwnerByActorId(actorId)) {
        return mActorTypeCache.emplace(actorId, eGameObjectsType::LOOT).first->second;
    } else {
        ext_assert(false, ("Game object type is UNDEFINED for actor ID " + std::to_string(actorId)).c_str());
        return eGameObjectsType::UNDEFINED;
    }
}

eGameObjectsCollisionType CombatActorsPoolHandler::GetGameObjectsCollisionType(
    const eGameObjectsType firstObject, const eGameObjectsType secondObject) const
{
    if ((eGameObjectsType::SPACESHIP == firstObject && eGameObjectsType::TOWER_MISSILE == secondObject)
        || (eGameObjectsType::TOWER_MISSILE == firstObject && eGameObjectsType::SPACESHIP == secondObject))
        return eGameObjectsCollisionType::SPACESHIP_WITH_MISSILE;

    if ((eGameObjectsType::SPACESHIP == firstObject && eGameObjectsType::NEUTRAL_SPACE_OBJECT == secondObject)
        || (eGameObjectsType::NEUTRAL_SPACE_OBJECT == firstObject && eGameObjectsType::SPACESHIP == secondObject))
        return eGameObjectsCollisionType::SPACESHIP_WITH_NEUTRAL_SPACE_OBJECT;

    if ((eGameObjectsType::TOWER_MISSILE == firstObject && eGameObjectsType::NEUTRAL_SPACE_OBJECT == secondObject)
        || (eGameObjectsType::NEUTRAL_SPACE_OBJECT == firstObject && eGameObjectsType::TOWER_MISSILE == secondObject))
        return eGameObjectsCollisionType::MISSILE_WITH_NEUTRAL_SPACE_OBJECT;

    if ((eGameObjectsType::TOWER_MISSILE == firstObject && eGameObjectsType::BARRIER == secondObject)
        || (eGameObjectsType::BARRIER == firstObject && eGameObjectsType::TOWER_MISSILE == secondObject))
        return eGameObjectsCollisionType::MISSILE_WITH_BARRIER;

    if ((eGameObjectsType::SPACESHIP_MISSILE == firstObject && eGameObjectsType::BARRIER == secondObject)
        || (eGameObjectsType::BARRIER == firstObject && eGameObjectsType::SPACESHIP_MISSILE == secondObject))
        return eGameObjectsCollisionType::ENEMY_MISSILE_WITH_BARRIER;

    return eGameObjectsCollisionType::UNDEFINED;
}

std::vector<std::shared_ptr<PhysicsComponent>> CombatActorsPoolHandler::GetSpaceStationsPhysicsComponents() const
{
    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    physicsComponents.reserve(mSpaceStations.size());
    std::transform(
        mSpaceStations.cbegin(), mSpaceStations.cend(), std::back_inserter(physicsComponents), [](const auto& spaceStationActor) {
            ext_assert(spaceStationActor->GetPhysicsComponent(), "Space station physics component is null");
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
            ext_assert(spaceship->GetPhysicsComponent(), "Spaceship physics component is null");
            return spaceship->GetPhysicsComponent();
        });
    return physicsComponents;
}

std::vector<std::shared_ptr<::EnginePhysics::PhysicsComponent>>
CombatActorsPoolHandler::GetMissilePhysicsComponents(const std::unordered_set<eMissileType>& missileTypes) const
{
    if (missileTypes.empty()) {
        return {};
    }

    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    for (const auto& missile : mMissilesPool) {
        const auto missileType = missile->GetMissileType();
        if (missileTypes.contains(missileType)
            && (missileType != eMissileType::FREEZING_RAY && missileType != eMissileType::ELECTRO_RAY)) {
            if (eMissileType::BLACK_HOLE == missileType) {
                const auto& gravityBombMissile = std::static_pointer_cast<GravityBombMissileActor>(missile);
                ext_assert(
                    physicsComponents.emplace_back(gravityBombMissile->GetCombatActivePhaseActor()->GetPhysicsComponent()),
                    "Failed to get combat active phase physics component in GetMissilePhysicsComponents");
                ext_assert(
                    physicsComponents.emplace_back(gravityBombMissile->GetExplosionPhaseActor()->GetPhysicsComponent()),
                    "Failed to get explosion phase physics component in GetMissilePhysicsComponents");
            } else {
                ext_assert(
                    physicsComponents.emplace_back(missile->GetPhysicsComponent()),
                    "Failed to get physics component in GetMissilePhysicsComponents");
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

std::vector<std::shared_ptr<PhysicsComponent>> CombatActorsPoolHandler::GetBarriersPhysicsComponents() const
{
    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    for (const auto& barrier : mBarriersPool) {
        const auto& pillarComponents = barrier->GetPillarPhysicsComponents();
        physicsComponents.insert(physicsComponents.end(), pillarComponents.begin(), pillarComponents.end());
    }
    return physicsComponents;
}

std::vector<std::shared_ptr<PhysicsComponent>> CombatActorsPoolHandler::GetLootPhysicsComponents() const
{
    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    for (const auto& loot : mLootActors) {
        ext_assert(loot->GetPhysicsComponent(), "Loot physics component is null");
        physicsComponents.emplace_back(loot->GetPhysicsComponent());
    }
    return physicsComponents;
}

std::vector<std::shared_ptr<PhysicsComponent>>
CombatActorsPoolHandler::GetPhysicsComponentsByGameObjectType(const eGameObjectsType gameObjectType) const
{
    // SPACESHIP, TOWER_MISSILE, SPACESHIP_MISSILE, NEUTRAL_SPACE_OBJECT, SPACE_STATION, BARRIER, LOOT
    switch (gameObjectType) {
    case eGameObjectsType::SPACESHIP:
        return GetSpaceShipsPhysicsComponents();
    case eGameObjectsType::TOWER_MISSILE:
    case eGameObjectsType::SPACESHIP_MISSILE:
        return GetMissilePhysicsComponents({eMissileType::BOMB, eMissileType::FREEZING_BOMB, eMissileType::BLACK_HOLE});
    case eGameObjectsType::NEUTRAL_SPACE_OBJECT:
        return GetSpaceStationsPhysicsComponents();
    case eGameObjectsType::SPACE_STATION:
        return GetSpaceStationsPhysicsComponents();
    case eGameObjectsType::BARRIER:
        return GetBarriersPhysicsComponents();
    case eGameObjectsType::LOOT:
        return GetLootPhysicsComponents();
    default:
        ext_assert(false, "Unsupported game object type: " + std::to_string(static_cast<int32_t>(gameObjectType)));
        return {};
    }
}

std::vector<std::shared_ptr<PhysicsComponent>>
CombatActorsPoolHandler::GetPhysicsComponentsByGameObjectTypes(const std::unordered_set<eGameObjectsType>& gameObjectTypes) const
{
    std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
    for (const auto& gameObjectType : gameObjectTypes) {
        const auto componentsForType = GetPhysicsComponentsByGameObjectType(gameObjectType);
        physicsComponents.insert(physicsComponents.end(), componentsForType.begin(), componentsForType.end());
    }
    return physicsComponents;
}

void CombatActorsPoolHandler::SpawnLoot(const int32_t count, const eLootCategory lootCategory)
{
    const auto& sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "Scene pointer is null in SpawnLoot");
    const auto& lootFactory = std::make_unique<LootFactory>();
    for (int32_t i = 0; i < count; ++i) {
        const auto& loot = mLootActors.emplace_back(
            lootFactory->CreateLoot(sceneSp, glm::vec3(-100000), glm::vec3(), Game::Constants::c_lootSize, lootCategory));
        loot->SetLootState(eLootState::IDLE);
    }
}

std::shared_ptr<LootActor> CombatActorsPoolHandler::GetFreeLootActor(const eLootCategory lootCategory)
{
    auto idleLootIt = std::find_if(mLootActors.cbegin(), mLootActors.cend(), [lootCategory](const auto& loot) {
        return loot->GetLootState() == eLootState::IDLE && loot->GetLootCategory() == lootCategory;
    });
    if (idleLootIt == mLootActors.cend()) {
        SpawnLoot(2, lootCategory);
    }

    idleLootIt = std::find_if(mLootActors.cbegin(), mLootActors.cend(), [lootCategory](const auto& loot) {
        return loot->GetLootState() == eLootState::IDLE && loot->GetLootCategory() == lootCategory;
    });

    ext_assert(idleLootIt != mLootActors.cend(), "Failed to find free loot actor after spawning new ones");

    return *idleLootIt;
}

std::vector<std::shared_ptr<Actor>>
CombatActorsPoolHandler::GetActorsByGameObjectType(const eGameObjectsType gameObjectType) const
{
    std::vector<std::shared_ptr<Actor>> actors;
    switch (gameObjectType) {
    case eGameObjectsType::SPACESHIP:
        actors.insert(actors.end(), mEnemySpaceships.cbegin(), mEnemySpaceships.cend());
        break;
    case eGameObjectsType::TOWER_MISSILE:
    case eGameObjectsType::SPACESHIP_MISSILE:
        actors.insert(actors.end(), mMissilesPool.cbegin(), mMissilesPool.cend());
        break;
    case eGameObjectsType::NEUTRAL_SPACE_OBJECT:
        actors.insert(actors.end(), mSpaceObjectsPool.cbegin(), mSpaceObjectsPool.cend());
        break;
    case eGameObjectsType::SPACE_STATION:
        actors.insert(actors.end(), mSpaceStations.cbegin(), mSpaceStations.cend());
        break;
    case eGameObjectsType::BARRIER:
        actors.insert(actors.end(), mBarriersPool.cbegin(), mBarriersPool.cend());
        break;
    case eGameObjectsType::LOOT:
        actors.insert(actors.end(), mLootActors.cbegin(), mLootActors.cend());
        break;
    default:
        ext_assert(false, "Unsupported game object type in GetActorsByGameObjectType");
        break;
    }
    return actors;
}

std::shared_ptr<IHighlightable>
CombatActorsPoolHandler::GetHighlightableByType(const int32_t goId, const eGameObjectsType goType) const
{
    if (eGameObjectsType::BARRIER == goType) {
        return GetBarrierOwnerActorById(goId);
    } else if (eGameObjectsType::SPACE_STATION == goType) {
        return GetSpaceStationOwnerActorById(goId);
    } else {
        return nullptr;
    }
}
} // namespace Game
