#include "SceneController.h"
#include "SpaceShipPlayerController.h"
#include "CombatController.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/AudioComponents/StreamingSoundComponent.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace EnginePhysics;
using namespace EngineCore;

namespace Game
{
    SceneController::SceneController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mCombatController(std::make_unique<CombatController>(scene)),
          mAmbientMusicDummy(std::make_shared<Actor>("Ambient Music Dummy",
                                                     std::make_shared<SceneComponent>("AmbientMusicDummyRootComponent",
                                                                                      glm::vec3(),
                                                                                      glm::vec3(),
                                                                                      glm::vec3())))
    {
    }

    SceneController::~SceneController()
    {
    }

    void SceneController::OnPreLevelInit()
    {
        mCombatController->OnPreLevelInit();
    }

    void SceneController::OnInitLevel()
    {
        mCombatController->OnInitLevel();
    }

    void SceneController::OnPostLevelInit()
    {
        if (const auto &sceneSp = mScene.lock())
        {
            sceneSp->AddActor(mAmbientMusicDummy);
            mAmbientMusicDummy->SetScene(mScene);
            ComponentData mAmbientMusicData("c_ambientMusic");
            const auto musicComponentCreator = std::make_shared<AudioComponentCreator<StreamingSoundComponent>>();
            const auto c_streamingMusic = std::static_pointer_cast<StreamingSoundComponent>(
                sceneSp->CreateComponent_GameThread(musicComponentCreator, mAmbientMusicData));
            c_streamingMusic->CreateStreamingSoundSource("piano-loop2.wav");
            c_streamingMusic->SetIsLoopSound(true);
            c_streamingMusic->SetGain(0.1f);
            mAmbientMusicDummy->AddComponent(c_streamingMusic);
        }

        mCombatController->OnPostLevelInit();
    }

    void SceneController::PostPlayLevelFinished()
    {
        mCombatController->PostPlayLevelFinished();

        //mAmbientMusicDummy->GetComponentsByType<StreamingSoundComponent>().back()->PlayStream();
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