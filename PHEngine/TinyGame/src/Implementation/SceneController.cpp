#include "SceneController.h"
#include "SpaceShipPlayerController.h"
#include "CombatController.h"
#include "Core/GameCore/Scene.h"

using namespace EnginePhysics;

namespace Game
{
    SceneController::SceneController(const std::weak_ptr<Scene> &scene)
        : mCombatController(std::make_unique<CombatController>(scene))
    {
    }

    SceneController::~SceneController()
    {
    }

    void SceneController::PreInit()
    {
        mCombatController->PreInit();
    }

    void SceneController::PostInit()
    {
        mCombatController->PostInit();
    }

    void SceneController::PostPlayLevelFinished()
    {
        mCombatController->PostPlayLevelFinished();
    }

    void SceneController::SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController)
    {
        mCombatController->SetPlayerActorController(mainPlayerActorController);
    }

    void SceneController::Tick(const float deltaTime)
    {
        mCombatController->Tick(deltaTime);
    }
}