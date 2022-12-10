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
            uiImage->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, mTestCanvas->GetName());
            uiImage->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, mTestCanvas->GetName());
            uiImage->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, mTestCanvas->GetName());
            uiImage->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, mTestCanvas->GetName());
            uiImage->SetTextureSrc("path.png");
            uiImage->SetOpacity(1);
            uiImage->SetZOrder(1);

            const auto &uiImage1 = std::make_shared<UiImage>(mTestCanvas, uiImage);
            uiImage1->SetWidth(100);
            uiImage1->SetHeight(100);
            uiImage1->SetAnchor(eUiAnchor::LEFT, eUiAnchor::LEFT, uiImage->GetName());
            uiImage1->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, uiImage->GetName());
            uiImage1->SetTextureSrc("grass.png");
            uiImage1->SetZOrder(2);

            const auto &uiImage2 = std::make_shared<UiImage>(mTestCanvas, uiImage);
            uiImage2->SetWidth(100);
            uiImage2->SetAnchor(eUiAnchor::LEFT, eUiAnchor::RIGHT, uiImage1->GetName());
            uiImage2->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, uiImage->GetName());
            uiImage2->SetAnchor(eUiAnchor::TOP, eUiAnchor::TOP, uiImage->GetName());
            uiImage2->SetTextureSrc("grass.png");
            uiImage2->SetZOrder(2);

            const auto &uiImage3 = std::make_shared<UiImage>(mTestCanvas, uiImage);
            uiImage3->SetWidth(200);
            uiImage3->SetHeight(200);
            uiImage3->SetAnchor(eUiAnchor::RIGHT, eUiAnchor::RIGHT, uiImage->GetName());
            uiImage3->SetAnchor(eUiAnchor::BOTTOM, eUiAnchor::BOTTOM, uiImage->GetName());
            uiImage3->SetTextureSrc("grass.png");
            uiImage3->SetZOrder(2);

            mTestCanvas->AddUiItem(uiImage);
            uiImage->AddUiItem(uiImage1);
            uiImage->AddUiItem(uiImage2);
            uiImage->AddUiItem(uiImage3);
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