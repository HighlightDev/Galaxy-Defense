#include "SceneController.h"
#include "SpaceShipPlayerController.h"
#include "CombatController.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/AudioComponents/StreamingSoundComponent.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace EnginePhysics;
using namespace EngineCore;
using namespace EngineUtility;
using namespace Graphics;
using namespace EngineCore::GUI;

namespace Game
{
    std::shared_ptr<::EngineCore::GUI::UiCanvas> mTestCanvas;

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

    void SceneController::OnLevelInit()
    {
        mCombatController->OnLevelInit();
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
                ComponentData mAmbientMusicData("c_ambientMusic");
                const auto musicComponentCreator = std::make_shared<AudioComponentCreator<StreamingSoundComponent>>();
                const auto c_streamingMusic = std::static_pointer_cast<StreamingSoundComponent>(
                    sceneSp->CreateComponent_GameThread(musicComponentCreator, mAmbientMusicData));
                c_streamingMusic->CreateStreamingSoundSource("piano-loop2.wav");
                c_streamingMusic->SetIsLoopSound(true);
                c_streamingMusic->SetGain(0.1f);
                mAmbientMusicDummy->AddComponent(c_streamingMusic);
            }
        }
#endif

        mCombatController->OnPostLevelInit();
    }

    void SceneController::PostPlayLevelFinished()
    {
        mCombatController->PostPlayLevelFinished();

#ifdef DEBUG
        if (EngineConfigHolder::GetInstance()->GetEngineConfig().EnableAmbientMusic)
        {
            mAmbientMusicDummy->GetComponentsByType<StreamingSoundComponent>().back()->PlayStream();
        }
#endif

        if (const auto &sceneSp = mScene.lock())
        {
            const auto &uiHandler = sceneSp->GetUiHandler();
            mTestCanvas = uiHandler->CreateCanvas(ViewPortInfo(0, 0, 600, 600));
            const auto &uiImage = std::make_shared<UiImage>(mTestCanvas, mTestCanvas);
            uiImage->SetWidth(100);
            uiImage->SetHeight(100);
            uiImage->SetAbsoluteOrigin(glm::ivec2(300, 300));
            uiImage->SetTextureSrc("fern.png");
            uiImage->SetOpacity(0.5f);
            uiImage->SetZOrder(1);
            mTestCanvas->AddUiItem(uiImage);
        }
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