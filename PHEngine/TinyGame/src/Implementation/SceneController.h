#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Actor.h"
#include "AiActorController.h"
#include "Events/MainPlayerActionEvent.h"
#include "Core/GameCore/BoundingBox.h"
#include "SpaceShipPlayerController.h"

#include <memory>
#include <utility>

using namespace EngineCore;

namespace Game
{
    enum class eBulletState
    {
        IDLE,
        ACTIVE,   
    };

    class SceneController : public ITickable,
                            public MainPlayerActionEvent
    {
        std::weak_ptr<Scene> mScene;

        std::weak_ptr<Actor> mMainPlayerShip;

        std::weak_ptr<SpaceShipPlayerController> mMainPlayerActorController;

        std::vector<std::weak_ptr<Actor>> mEnemies;

        std::vector<std::weak_ptr<AiActorController>> mEnemyActorControllers;

        std::vector<std::pair<std::weak_ptr<Actor>, eBulletState>> mWeaponBulletsPool;

        size_t enemyShipCounter = 0;

        size_t bulletCounter = 0;

        std::shared_ptr<Actor> mDummyBullet;

        float mCoolDownTime = 0.2f;

        bool bIsCoolDownInProgress = false;

        BoundingBox mLevelBounds;

    public:
        SceneController(const std::weak_ptr<Scene> &scene);

        ~SceneController();

        virtual void PreInit();

        virtual void PostInit();

        virtual void PostPlayLevelFinished();

        virtual void Tick(const float deltaTime) override;

        void SetPlayerShipActor(const std::weak_ptr<Actor> &mainPlayerShip);

        void SetPlayeActorController(const std::weak_ptr<SpaceShipPlayerController>& mainPlayerActorController);

    protected:
        virtual void ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data) override;

    private:
        std::shared_ptr<Actor> CreateEnemySpaceShip(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                    const glm::vec3 &rotation, const glm::vec3 &scale);

        std::shared_ptr<Actor> CreateWeaponBullet(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                  const glm::vec3 &rotation, const glm::vec3 &scale);

        void CreateWeaponBulletPool(const size_t poolSize, const std::shared_ptr<Scene>& sceneSp);

        void ShootBullet(const glm::vec3& bulletStartPosition);

        void FlushToPoolUsedBullets();
    };
}