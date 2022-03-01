#include "EnemySceneController.h"

namespace Game
{
    EnemySceneController::EnemySceneController(const std::weak_ptr<Scene> &scene)
        : mScene(scene)
        , mEnemies()
        , mEnemyActorControllers()
    {
    }

    void EnemySceneController::PreInit()
    {
    }

    void EnemySceneController::PostInit()
    {
    }

    void EnemySceneController::Tick(const float deltaTime)
    {
    }
}