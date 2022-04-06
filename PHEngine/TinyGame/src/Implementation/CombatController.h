#pragma once

#include "Core/GameCore/ITickable.h"
#include "Events/MainPlayerActionEvent.h"
#include "Core/GameCore/BoundingBox.h"
#include "Core/GameCore/Event/PhysicsCollisionOccuredEvent.h"
#include "CombatEntity.h"

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

    enum class eBulletState
    {
        IDLE,
        ACTIVE,
    };

    class CombatController : public ITickable,
                             public MainPlayerActionEvent,
                             public PhysicsCollisionOccuredEvent
    {
        std::weak_ptr<Scene> mScene;

        std::shared_ptr<Actor> mPlayerShip;

        std::shared_ptr<SpaceShipPlayerController> mMainPlayerActorController;

        std::vector<CombatEntity> mEnemies;

        std::vector<std::pair<std::shared_ptr<Actor>, eBulletState>> mWeaponBulletsPool;

        size_t enemyShipCounter = 0;

        size_t bulletCounter = 0;

        float mCoolDownTime = 0.2f;

        bool bIsCoolDownInProgress = false;

        BoundingBox mLevelBounds;

    public:
        CombatController(const std::weak_ptr<Scene> &scene);

        ~CombatController();

        virtual void PreInit();

        virtual void PostInit();

        virtual void PostPlayLevelFinished();

        virtual void Tick(const float deltaTime) override;

        void SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController);

    protected:
        virtual void ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data) override;

        virtual void ProcessEvent(const typename PhysicsCollisionOccuredEvent::EventData_t &data) override;

    private:
        void CreateWeaponBulletPool(const size_t poolSize, const std::shared_ptr<Scene> &sceneSp);

        void ShootBullet(const glm::vec3 &bulletStartPosition);

        void FlushToPoolUsedBullets();

        typename std::vector<CombatEntity>::iterator FindEnemyShipByName(const std::string &actorName);

        typename std::vector<CombatEntity>::iterator FindEnemyShipById(const uint64_t actorId);

        typename std::vector<std::pair<std::shared_ptr<Actor>, eBulletState>>::iterator FindBulletByName(const std::string &actorName);

        typename std::vector<std::pair<std::shared_ptr<Actor>, eBulletState>>::iterator FindBulletById(const uint64_t actorId);
    };
}