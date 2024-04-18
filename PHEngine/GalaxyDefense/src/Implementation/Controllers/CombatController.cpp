#include "CombatController.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/LoggerExtension.h"

#include "Implementation/Factories/WeakSpaceShipFactory.h"
#include "Implementation/Factories/AsteroidFactory.h"
#include "Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h"
#include "Implementation/Modifiers/ElectroRayChainModifier.h"
#include "Implementation/GalaxySceneCamera.h"
#include "Implementation/Events/MainPlayerStatusChangedEvent.h"
#include "Implementation/DataProviders/PlayerDataProvider.h"
#include "Implementation/MissileType.h"
#include "Implementation/Levels/LevelSerializationHelper.h"
#include "Implementation/Navigation/PathSegment.h"
#include "Implementation/Navigation/Path.h"
#include "Implementation/Factories/SpaceStationFactory.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include <array>
#include <unordered_map>
#include <tuple>

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore;
using namespace Resources;

#undef min
#undef max

namespace Game
{
    float mDeltaTime = 0.0f;

    CombatController::CombatController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mLevelBounds(BoundingBox3D(glm::vec3(0), glm::vec3(100, 50, 100))),
          mNavigationController(std::make_shared<NavigationController>(scene)),
          mCombatActorsPoolHandler(std::make_shared<CombatActorsPoolHandler>(scene))
    {
    }

    CombatController::~CombatController()
    {
        SphereContactCollisionEvent::GetInstance()->RemoveListener(SphereContactCollisionEvent::GetInstanceId());
        MainPlayerActionEvent::GetInstance()->RemoveListener(MainPlayerActionEvent::GetInstanceId());
        PhysicsCollisionGameThreadEvent::GetInstance()->RemoveListener(PhysicsCollisionGameThreadEvent::GetInstanceId());
        RayCollisionEvent::GetInstance()->RemoveListener(RayCollisionEvent::GetInstanceId());
    }

    void CombatController::InitFromLevelData(const LevelData &levelData)
    {
        assert(levelData.isDataValid());
        const auto &sceneSp = mScene.lock();
        assert(sceneSp);

        mLevelBounds = BoundingBox3D(glm::vec3(levelData.LevelBoundaryOrigin.x, 0.0f, levelData.LevelBoundaryOrigin.y),
                                     glm::vec3(levelData.LevelBoundaryExtent.x, 50.0f, levelData.LevelBoundaryExtent.y));

        std::unordered_map<std::string, Path> pathRoutes;

        for (const auto &[routeName, route] : levelData.RoutesData)
        {
            Path path;
            PathSegment segment;
            segment.SetSubdivisionsCount(50);
            for (const auto &routeControlPoint : route)
            {
                segment.SetControlPoints({{std::get<0>(routeControlPoint), std::get<1>(routeControlPoint), std::get<2>(routeControlPoint)}});
                path.AppendPathSegmentToTheEnd(segment);
            }
            pathRoutes.emplace(routeName, path);
        }
        mNavigationController->SetPathRoutes(pathRoutes);

        SpaceStationFactory spaceStationFactory;
        for (const auto &[towerName, towerData] : levelData.TowersData)
        {
            const glm::vec3 &position = std::get<0>(towerData);
            const glm::vec3 &scale = std::get<1>(towerData);
            mCombatActorsPoolHandler->CreateSpaceStationActor(towerName, position, glm::vec3(), scale);
        }
    }

    void CombatController::OnPreLevelInit()
    {
        const auto thisSp = shared_from_this();
        SphereContactCollisionEvent::GetInstance()->AddListener(thisSp);
        MainPlayerActionEvent::GetInstance()->AddListener(thisSp);
        PhysicsCollisionGameThreadEvent::GetInstance()->AddListener(thisSp);
        RayCollisionEvent::GetInstance()->AddListener(thisSp);
        mNavigationController->OnPreLevelInit();
    }

    void CombatController::OnLevelInit()
    {
        mCombatActorsPoolHandler->SpawnEnemySpaceships(10);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::BOMB, 10);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::FREEZING, 3);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::ELECTRO_RAY, 1);
        mCombatActorsPoolHandler->SpawnMissiles(eMissileType::BLACK_HOLE, 2);

        const std::unordered_map<eMissileType, size_t> availabeMissileTypes = {
            {eMissileType::BOMB, 10},
            {eMissileType::FREEZING, 3},
            {eMissileType::ELECTRO_RAY, 1},
            {eMissileType::BLACK_HOLE, 2}};
        PlayerDataProvider::GetInstance()->SetAvailableMissileTypes(availabeMissileTypes);

        mCombatActorsPoolHandler->SpawnAsteroids(20);
        mNavigationController->OnLevelInit();
    }

    void CombatController::OnPostLevelInit()
    {
    }

    void CombatController::PostPlayLevelFinished()
    {
        mNavigationController->PostPlayLevelFinished();

        const auto &pathNames = mNavigationController->GetPathNames();
        for (const auto &pathName : pathNames)
        {
            const auto &freeShip = mCombatActorsPoolHandler->GetFreeSpaceshipActor();
            mNavigationController->PutSpaceshipOnRoute(pathName, freeShip);
        }
    }

    void CombatController::ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data)
    {
        const auto &playerAction = std::get<0>(data);

        if (eMainPlayerActionEnum::SHOOT == playerAction)
        {
            if (const auto &sceneSp = mScene.lock())
            {
                ShootBullet(mPlayerShip->GetRootComponent()->GetTranslation());
                // todo: use player data provider instead
                LuaMainPlayerStatusChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, eMainPlayerStatusType::LIFE_POINTS_CHANGED);
            }
        }
        else if (eMainPlayerActionEnum::SELECT_NEXT_MISSILE_TYPE == playerAction || eMainPlayerActionEnum::SELECT_PREV_MISSILE_TYPE == playerAction)
        {
            static std::array<eMissileType, 4> missiles = {eMissileType::BOMB, eMissileType::FREEZING, eMissileType::ELECTRO_RAY, eMissileType::BLACK_HOLE};
            const auto &playerDataProvider = PlayerDataProvider::GetInstance();
            const auto selectedMissileType = playerDataProvider->GetSelectedMissileType();
            const auto foundMissileTypeIt = std::find(missiles.cbegin(), missiles.cend(), selectedMissileType);
            assert(foundMissileTypeIt != missiles.cend());
            const auto selectedMissileTypeIndex = std::distance(missiles.cbegin(), foundMissileTypeIt);
            const auto newMissileType = eMainPlayerActionEnum::SELECT_NEXT_MISSILE_TYPE == playerAction
                                            ? (missiles.size() - 1) == selectedMissileTypeIndex ? missiles.at(0) : missiles.at(selectedMissileTypeIndex + 1)
                                        : eMainPlayerActionEnum::SELECT_PREV_MISSILE_TYPE == playerAction
                                            ? 0 == selectedMissileTypeIndex ? missiles.at(missiles.size() - 1) : missiles.at(selectedMissileTypeIndex - 1)
                                            : eMissileType::NONE;
            assert(newMissileType != eMissileType::NONE);
            playerDataProvider->SetSelectedMissileType(newMissileType);
        }
    }

    void CombatController::ProcessEvent(const typename PhysicsCollisionGameThreadEvent::EventData_t &data)
    {
        const ePhysicsCollisionStateType collisionEventType = std::get<0>(data);
        const ePhysicsBodyType physBodyType = std::get<1>(data);
        const auto this_actor_id = std::get<3>(data);
        const auto that_actor_id = std::get<5>(data);

        const auto &sceneSp = mScene.lock();
        if (!sceneSp || ePhysicsBodyType::GHOST != physBodyType)
            return;

        const std::string collisionType = ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType
                                              ? "collision registered"
                                              : "collision unregister";

        const auto thisActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(this_actor_id);
        const auto thatActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(that_actor_id);
        const auto objectsCollisionType = mCombatActorsPoolHandler->GetGameObjectsCollisionType(thisActorGameObjectType, thatActorGameObjectType);

        if (objectsCollisionType == eGameObjectsCollisionType::UNDEFINED)
            return;
        if (eGameObjectsCollisionType::SPACESHIP_WITH_MISSILE == objectsCollisionType)
        {
            const auto &ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType
                                                  ? mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(this_actor_id)
                                                  : mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(that_actor_id);

            const auto &ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType
                                                ? mCombatActorsPoolHandler->GetMissileOwnerActorById(this_actor_id)
                                                : mCombatActorsPoolHandler->GetMissileOwnerActorById(that_actor_id);

            const auto spaceshipActor_id = eGameObjectsType::SPACESHIP == thisActorGameObjectType ? this_actor_id : that_actor_id;
            const auto missileActor_id = eGameObjectsType::MISSILE == thisActorGameObjectType ? this_actor_id : that_actor_id;

            const auto &concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
                                                   ? ownerMissileActor
                                                   : ownerMissileActor->GetChildByObjectId(missileActor_id);

            const auto &concreteSpaceshipActor = ownerEnemyShipActor->GetObjectId() == spaceshipActor_id
                                                     ? ownerEnemyShipActor
                                                     : ownerEnemyShipActor->GetChildByObjectId(spaceshipActor_id);

            LogInfo("CombatController::PhysicsCollisionGameThreadEvent =>", collisionType, "missile with spaceship, this_actor = ", concreteMissileActor->GetName(), " that_actor = ",
                    concreteSpaceshipActor->GetName());

            const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
            if (ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType)
            {
                explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor);
            }
            else
            {
                explosionVisitor->EndExplosionForSpaceship(ownerEnemyShipActor, concreteMissileActor);
            }
        }
        else if (eGameObjectsCollisionType::SPACESHIP_WITH_NEUTRAL_SPACE_OBJECT == objectsCollisionType)
        {
            const auto &ownerEnemyShipActor = eGameObjectsType::SPACESHIP == thisActorGameObjectType
                                                  ? mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(this_actor_id)
                                                  : mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(that_actor_id);
            const auto &ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType
                                                    ? mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(this_actor_id)
                                                    : mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(that_actor_id);

            LogInfo("CombatController::PhysicsCollisionGameThreadEvent =>", collisionType, "spaceship with space object, this_actor = ", ownerEnemyShipActor->GetName(),
                    " that_actor = ", ownerSpaceObjectActor->GetName());

            ownerEnemyShipActor->TriggerDamageReceived(1UL, eDamageDealerType::NEUTRAL_OBJECT);
            ownerSpaceObjectActor->TriggerDisabled();
        }
        else if (eGameObjectsCollisionType::MISSILE_WITH_NEUTRAL_SPACE_OBJECT == objectsCollisionType)
        {
            const auto &ownerMissileActor = eGameObjectsType::MISSILE == thisActorGameObjectType
                                                ? mCombatActorsPoolHandler->GetMissileOwnerActorById(this_actor_id)
                                                : mCombatActorsPoolHandler->GetMissileOwnerActorById(that_actor_id);

            const auto &ownerSpaceObjectActor = eGameObjectsType::NEUTRAL_SPACE_OBJECT == thisActorGameObjectType
                                                    ? mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(this_actor_id)
                                                    : mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(that_actor_id);
            const auto &missileActor_id = eGameObjectsType::MISSILE == thisActorGameObjectType ? this_actor_id : that_actor_id;

            const auto &concreteMissileActor = ownerMissileActor->GetObjectId() == missileActor_id
                                                   ? ownerMissileActor
                                                   : ownerMissileActor->GetChildByObjectId(missileActor_id);

            LogInfo("CombatController::PhysicsCollisionGameThreadEvent => ", collisionType, "missile with space object, this_actor = ", concreteMissileActor->GetName(),
                    " that_actor = ", ownerSpaceObjectActor->GetName());

            const auto explosionVisitor = ownerMissileActor->CreateMissileExplosionVisitor();
            if (ePhysicsCollisionStateType::COLLISION_REGISTERED == collisionEventType)
            {
                explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor);
            }
            else
            {
                explosionVisitor->EndExplosionForSpaceObject(ownerSpaceObjectActor, concreteMissileActor);
            }
        }
    }

    void CombatController::ProcessEvent(const typename RayCollisionEvent::EventData_t &data)
    {
        const auto &eventSenderMissileWp = std::get<0>(data);
        const auto &collidedActorWp = std::get<1>(data);

        if (const auto &rayMissileActorSp = eventSenderMissileWp.lock())
        {
            if (const auto &collidedActorSp = collidedActorWp.lock())
            {
                const auto &gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedActorSp->GetObjectId());
                if (eGameObjectsType::UNDEFINED != gameObjectType)
                {
                    const auto explosionVisitor = rayMissileActorSp->CreateMissileExplosionVisitor();
                    if (eGameObjectsType::SPACESHIP == gameObjectType)
                    {
                        const auto &ownerEnemyShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceship(ownerEnemyShipActor, rayMissileActorSp);
                    }
                    else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType)
                    {
                        const auto &ownerSpaceObjectActor = mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(collidedActorSp->GetObjectId());
                        explosionVisitor->StartExplosionForSpaceObject(ownerSpaceObjectActor, rayMissileActorSp);
                    }
                }
            }
        }
    }

    void CombatController::ProcessEvent(const typename SphereContactCollisionEvent::EventData_t &data)
    {
        const auto &srcActorId = std::get<0>(data);
        const auto &collidedActorIds = std::move(std::get<1>(data));

        const auto &srcActorGameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(srcActorId);
        if (eGameObjectsType::UNDEFINED != srcActorGameObjectType && eGameObjectsType::MISSILE != srcActorGameObjectType)
        {
            const std::shared_ptr<Actor> &srcCollisionActor = eGameObjectsType::SPACESHIP == srcActorGameObjectType
                                                                  ? std::static_pointer_cast<Actor>(mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(srcActorId))
                                                              : eGameObjectsType::NEUTRAL_SPACE_OBJECT == srcActorGameObjectType
                                                                  ? std::static_pointer_cast<Actor>(mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(srcActorId))
                                                                  : nullptr;
            assert(srcCollisionActor);
            for (const auto &collidedActorId : collidedActorIds)
            {
                const auto &gameObjectType = mCombatActorsPoolHandler->GetGameObjectTypeByActorId(collidedActorId);
                if (eGameObjectsType::UNDEFINED != gameObjectType)
                {
                    if (eGameObjectsType::SPACESHIP == gameObjectType)
                    {
                        const auto &ownerEnemyShipActor = mCombatActorsPoolHandler->GetEnemyShipOwnerActorById(collidedActorId);
                        const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(std::make_pair(gameObjectType, ownerEnemyShipActor),
                                                                                                       std::make_pair(srcActorGameObjectType, srcCollisionActor));
                        electroRayChainModifier->Initialize(mCombatActorsPoolHandler->GetFreeElectroChainActor());
                        ownerEnemyShipActor->AddModifier(electroRayChainModifier);
                    }
                    else if (eGameObjectsType::NEUTRAL_SPACE_OBJECT == gameObjectType)
                    {
                        const auto &ownerSpaceObjectActor = mCombatActorsPoolHandler->GetSpaceObjectOwnerActorById(collidedActorId);
                        const auto electroRayChainModifier = std::make_shared<ElectroRayChainModifier>(std::make_pair(gameObjectType, ownerSpaceObjectActor),
                                                                                                       std::make_pair(srcActorGameObjectType, srcCollisionActor));

                        electroRayChainModifier->Initialize(mCombatActorsPoolHandler->GetFreeElectroChainActor());
                        ownerSpaceObjectActor->AddModifier(electroRayChainModifier);
                    }
                }
            }
        }
    }

    void CombatController::Tick(const float deltaTime)
    {
        FlushToPoolUsedBullets();
        UpdateMissilesData();

        mNavigationController->Tick(deltaTime);
    }

    void CombatController::ShootBullet(const glm::vec3 &bulletStartPosition)
    {
        const auto selectedMissileType = PlayerDataProvider::GetInstance()->GetSelectedMissileType();
        const auto &missile = mCombatActorsPoolHandler->GetFreeMissile(eMissileType::BOMB);
        missile->TriggerSpawn(bulletStartPosition, eDamageDealerType::MAIN_PLAYER, mPlayerShip);
    }

    void CombatController::FlushToPoolUsedBullets()
    {
        // for (auto &missile : mMissilesPool)
        // {
        //     if (eMissileActivityState::ACTIVE == missile->GetMissileActivityState())
        //     {
        //         if (!missile->IsInsideLevel(mLevelBounds))
        //         {
        //             missile->TriggerDisabled();
        //         }
        //     }
        // }
    }

    void CombatController::UpdateMissilesData()
    {
        // std::unordered_map<eMissileType, size_t> missiles;
        // const auto &availableMissileTypes = PlayerDataProvider::GetInstance()->GetAvailableMissileTypes();
        // for (const auto &avlMissileType : availableMissileTypes)
        // {
        //     missiles[avlMissileType] = 0;
        // }
        // for (auto &missile : mMissilesPool)
        // {
        //     if (eMissileActivityState::IDLE == missile->GetMissileActivityState())
        //     {
        //         missiles[missile->GetMissileType()] = missiles.at(missile->GetMissileType()) + 1;
        //     }
        // }
        // PlayerDataProvider::GetInstance()->SetMissilesCount(missiles);
    }

    void CombatController::CleanUp()
    {
    }
}