#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Actor.h"
#include "AiActorController.h"

#include <memory>

using namespace EngineCore;

namespace Game
{
    class EnemySceneController : public ITickable
    {
        std::weak_ptr<Scene> mScene;

        std::vector<std::weak_ptr<Actor>> mEnemies;

        std::vector<std::weak_ptr<AiActorController>> mEnemyActorControllers;

    public:

        EnemySceneController(const std::weak_ptr<Scene> &scene);

        virtual void PreInit();

        virtual void PostInit();

        virtual void Tick(const float deltaTime) override;
    };
}