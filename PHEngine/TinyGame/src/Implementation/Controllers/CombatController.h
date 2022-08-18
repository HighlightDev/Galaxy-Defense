#pragma once

#include "Core/GameCore/ITickable.h"
#include "Implementation/Events/MainPlayerActionEvent.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Core/GameCore/BoundingBox.h"
#include "Core/GameCore/Event/PhysicsCollisionEvent.h"

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
    class SpaceShipPlayerController;

    class CombatController : public ITickable,
                             public MainPlayerActionEvent,
                             public PhysicsCollisionEvent
    {
        std::weak_ptr<Scene> mScene;

        std::shared_ptr<Actor> mPlayerShip;

        std::shared_ptr<SpaceShipPlayerController> mMainPlayerActorController;

        std::vector<std::shared_ptr<SpaceshipActor>> mEnemies;

        std::vector<std::shared_ptr<MissileActor>> mMissilesPool;

        float mCoolDownTime = 0.2f;

        bool bIsCoolDownInProgress = false;

        BoundingBox mLevelBounds;

    public:
        CombatController(const std::weak_ptr<Scene> &scene);

        ~CombatController();

        virtual void OnPreLevelInit();

        virtual void OnLevelInit();

        virtual void OnPostLevelInit();

        virtual void PostPlayLevelFinished();

        virtual void Tick(const float deltaTime) override;

        void SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController);

    protected:
        virtual void ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data) override;

        virtual void ProcessEvent(const typename PhysicsCollisionEvent::EventData_t &data) override;

    private:
        void CreateWeaponBulletPool(const size_t poolSize, const std::shared_ptr<Scene> &sceneSp);

        void ShootBullet(const glm::vec3 &bulletStartPosition);

        void FlushToPoolUsedBullets();

        typename std::vector<std::shared_ptr<SpaceshipActor>>::iterator FindEnemyShipByName(const std::string &actorName);

        typename std::vector<std::shared_ptr<SpaceshipActor>>::iterator FindEnemyShipOwnerActorById(const uint64_t actorId);

        typename std::vector<std::shared_ptr<MissileActor>>::iterator FindBulletByName(const std::string &actorName);

        typename std::vector<std::shared_ptr<MissileActor>>::iterator FindBulletOwnerActorById(const uint64_t actorId);

        void TestSound();
    };
}