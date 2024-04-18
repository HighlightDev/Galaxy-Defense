#pragma once

#include "ILevelController.h"
#include "Core/GameCore/ITickable.h"
#include "Implementation/Events/MainPlayerActionEvent.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/Actors/BackgroundSpaceObjectActor.h"
#include "Implementation/Events/RayCollisionEvent.h"
#include "Implementation/Events/SphereContactCollisionEvent.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"
#include "Implementation/GameObjectsType.h"
#include "Implementation/GameObjectsCollisionType.h"
#include "Implementation/Controllers/NavigationController.h"
#include "Implementation/Levels/LevelData.h"
#include "Implementation/Actors/SpaceStationActor.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

#include <memory>
#include <utility>

using namespace EngineCore;
using namespace Event;

namespace EngineCore
{
    class Scene;
    class Actor;
}

namespace Game
{
    class CombatController : public ITickable,
                             public ILevelController,
                             public MainPlayerActionEvent,
                             public PhysicsCollisionGameThreadEvent,
                             public RayCollisionEvent,
                             public SphereContactCollisionEvent,
                             public std::enable_shared_from_this<CombatController>
    {
        std::weak_ptr<Scene> mScene;

        std::shared_ptr<Actor> mPlayerShip;

        std::shared_ptr<NavigationController> mNavigationController;

        std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

        BoundingBox3D mLevelBounds;


    public:
        CombatController(const std::weak_ptr<Scene> &scene);

        ~CombatController();

        void OnPreLevelInit() override;

        void OnLevelInit() override;

        void OnPostLevelInit() override;

        void PostPlayLevelFinished() override;

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override{};

        void CleanUp() override;

        void InitFromLevelData(const LevelData& levelData);

    protected:
        void ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data) override;

        void ProcessEvent(const typename PhysicsCollisionGameThreadEvent::EventData_t &data) override;

        void ProcessEvent(const typename RayCollisionEvent::EventData_t &data) override;

        void ProcessEvent(const typename SphereContactCollisionEvent::EventData_t &data) override;

    private:
        void ShootBullet(const glm::vec3 &bulletStartPosition);

        void FlushToPoolUsedBullets();

        void UpdateMissilesData();
    };
}