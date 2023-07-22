#include "SceneController.h"
#include "SpaceShipPlayerController.h"
#include "CombatController.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/AudioComponents/StreamingSoundComponent.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <iostream>

using namespace EnginePhysics;
using namespace EngineCore;
using namespace EngineUtility;
using namespace Graphics;
using namespace EngineCore::GUI;

namespace Game
{
    SceneController::SceneController(const std::weak_ptr<Scene> &scene)
        : mScene(scene),
          mCombatController(std::make_unique<CombatController>(scene)),
          mUiController(std::make_unique<UiController>(scene)),
          mAmbientMusicDummy(std::make_shared<Actor>("Ambient Music Dummy",
                                                     std::make_shared<SceneComponent>("AmbientMusicDummyRootComponent",
                                                                                      glm::vec3(),
                                                                                      glm::vec3(),
                                                                                      glm::vec3())))
    {
    }

    SceneController::~SceneController()
    {
        std::cout << "SceneController::dctor";
    }

    void SceneController::OnPreLevelInit()
    {
        mCombatController->OnPreLevelInit();
        mUiController->OnPreLevelInit();
    }

    void SceneController::OnLevelInit()
    {
        mCombatController->OnLevelInit();
        mUiController->OnLevelInit();
    }

    void SceneController::OnPostLevelInit()
    {
#ifdef DEBUG
        if (EngineConfigHolder::GetInstance()->GetEngineConfig().EnableAmbientMusic)
        {
            if (const auto &sceneSp = mScene.lock())
            {
                sceneSp->AddActor(mAmbientMusicDummy);
                mAmbientMusicDummy->SetScene(mScene);
                const auto musicComponentCreator = std::make_shared<AudioComponentCreator<StreamingSoundComponent>>();
                const auto c_streamingMusic = std::static_pointer_cast<StreamingSoundComponent>(
                    sceneSp->CreateComponent_GameThread(musicComponentCreator, std::make_shared<ComponentData>("c_ambientMusic")));
                c_streamingMusic->CreateStreamingSoundSource("piano-loop2.wav");
                c_streamingMusic->SetIsLoopSound(true);
                c_streamingMusic->SetGain(0.1f);
                mAmbientMusicDummy->AddComponent(c_streamingMusic);
            }
        }
#endif

        mCombatController->OnPostLevelInit();
        mUiController->OnPostLevelInit();
    }

    void SceneController::PostPlayLevelFinished()
    {
        mCombatController->PostPlayLevelFinished();
        mUiController->PostPlayLevelFinished();

#ifdef DEBUG
        if (EngineConfigHolder::GetInstance()->GetEngineConfig().EnableAmbientMusic)
        {
            mAmbientMusicDummy->GetComponentsByType<StreamingSoundComponent>().back()->PlayStream();
        }
#endif
    }

    void SceneController::SetPlayerActorController(const std::shared_ptr<SpaceShipPlayerController> &mainPlayerActorController)
    {
        mCombatController->SetPlayerActorController(mainPlayerActorController);
    }

    void SceneController::Tick(const float deltaTime)
    {
        mCombatController->Tick(deltaTime);
        mUiController->Tick(deltaTime);
    }

    void SceneController::UnpausableTick(const float deltaTime)
    {
        mCombatController->UnpausableTick(deltaTime);
        mUiController->UnpausableTick(deltaTime);
    }

    void SceneController::CleanUp()
    {
        mUiController->CleanUp();
        mCombatController->CleanUp();
    }
}