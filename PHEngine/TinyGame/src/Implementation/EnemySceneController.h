#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Actor.h"
#include "AiActorController.h"
#include "Core/GameCore/Input/KeyboardBindings.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    class EnemySceneController : public ITickable
    {
        std::weak_ptr<Scene> mScene;

        std::vector<std::weak_ptr<Actor>> mEnemies;

        std::vector<std::weak_ptr<AiActorController>> mEnemyActorControllers;

        size_t enemyShipCounter = 0;

        KeyboardBindings m_keyboardBindings;

    public:
        EnemySceneController(const std::weak_ptr<Scene> &scene);

        virtual void PreInit();

        virtual void PostInit();

        virtual void Tick(const float deltaTime) override;

        void SpawnEnemySpaceship(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale);

        void Trigger();

    private:
        std::shared_ptr<Actor> CreateEnemySpaceShip(const std::shared_ptr<Scene> &scene, const glm::vec3 &translation,
                                                    const glm::vec3 &rotation, const glm::vec3 &scale);
    };
}