#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Actor.h"
#include "AiActorController.h"
#include "Events/MainPlayerActionEvent.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    class SceneController : public ITickable,
                            public MainPlayerActionEvent
    {
        std::weak_ptr<Scene> mScene;

        std::weak_ptr<Actor> mMainPlayerShip;

        std::vector<std::weak_ptr<Actor>> mEnemies;

        std::vector<std::weak_ptr<AiActorController>> mEnemyActorControllers;

        std::vector<std::weak_ptr<Actor>> mWeaponBulletsPool;

        size_t enemyShipCounter = 0;

        size_t bulletCounter = 0;

        std::shared_ptr<Actor> mDummyBullet;

        float mCoolDownTime = 0.5f;
        bool bIsCoolDownInProgress = false;

    public:
        SceneController(const std::weak_ptr<Scene> &scene);

        ~SceneController();

        virtual void PreInit();

        virtual void PostInit();

        virtual void Tick(const float deltaTime) override;

        void SetPlayerShipActor(const std::weak_ptr<Actor> &mainPlayerShip);

    protected:
        virtual void ProcessEvent(const typename MainPlayerActionEvent::EventData_t &data) override;

    private:
        std::shared_ptr<Actor> CreateEnemySpaceShip(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                    const glm::vec3 &rotation, const glm::vec3 &scale);

        std::shared_ptr<Actor> CreateWeaponBullet(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                  const glm::vec3 &rotation, const glm::vec3 &scale);
    };
}