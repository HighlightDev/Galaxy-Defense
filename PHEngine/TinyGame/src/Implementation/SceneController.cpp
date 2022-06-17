#include "SceneController.h"
#include "SpaceShipPlayerController.h"
#include "CombatController.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/Text/FontHandler.h"
#include "Core/GameCore/GUI/Text/FontParams.h"
#include "Core/GameCore/GUI/Text/TextField.h"

#include <glm/vec2.hpp>

using namespace EnginePhysics;
using namespace EngineCore;

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

    std::shared_ptr<TextField> text1, text2, text3;
    float angle = 0.0f;
    int counter = 0;

    void SceneController::PostInit()
    {
        mCombatController->PostInit();

        text1 = std::make_shared<TextField>("arial", 5, "Hello world!", glm::vec3(1, 0.5, 0.5), glm::vec2(0.5), 0.5, 5, false);
        text2 = std::make_shared<TextField>("arial", 5, "Hi!", glm::vec3(0.1, 0.1, 0.5), glm::vec2(0.2), 0.5, 5, false);
        text3 = std::make_shared<TextField>("arial", 5, "Privet pipka", glm::vec3(1), glm::vec2(), 0.5, 5, false);
        text1->RegisterText();
        text2->RegisterText();
        text3->RegisterText();
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

        static constexpr double deg_to_rad = 3.15 / 180;
        if (counter == 1000)
        {
            float x = std::cos(deg_to_rad * angle) * 0.5f + 0.5f;
            float y = std::sin(deg_to_rad * angle) * 0.5f + 0.5f;
            text1->SetPosition(glm::vec2(x, y));
            counter = 0;
        }

        if (counter == 800)
        {
            text1->SetText(std::to_string(deltaTime * 100.0f) + "_text");
        }

        angle += 0.5f;
        angle = std::fmod(angle, 360.0f);
        ++counter;
    }
}