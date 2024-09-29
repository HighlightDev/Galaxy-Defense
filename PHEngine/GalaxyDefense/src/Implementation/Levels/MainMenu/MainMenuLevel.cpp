#include "MainMenuLevel.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaEngineScriptExecutor.h"

#include "Core/GameCore/Components/AudioComponents/StreamingSoundComponent.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

using namespace IO;
using namespace EngineCore;
using namespace EngineCore::Scripts;

namespace Game
{

   MainMenuLevel::MainMenuLevel()
       : LevelBase("MainMenuLevel"),
         mAmbientMusicDummy(std::make_shared<Actor>("Ambient Music Dummy",
                                                    std::make_shared<SceneComponent>("AmbientMusicDummyRootComponent",
                                                                                     glm::vec3(),
                                                                                     glm::vec3(),
                                                                                     glm::vec3())))

   {
   }

   MainMenuLevel::~MainMenuLevel()
   {
   }

   void MainMenuLevel::PreLevelInit()
   {
      Base::PreLevelInit();
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      mUiController = std::make_unique<MainMenuLevelUiController>(sceneSp);
      mUiController->OnPreLevelInit();
   }

   void MainMenuLevel::CreateScene()
   {
      RunLuaBuildLevelScript();
   }

   void MainMenuLevel::RunLuaBuildLevelScript()
   {
      const auto sceneSp = mSceneWp.lock();
      assert(sceneSp);
      const auto lvlName = std::string("/LevelCreators/MainMenuLvl.lua");
      LuaEngineScriptExecutor mLuaLevelBuilder = LuaEngineScriptExecutor(lvlName);
      mLuaLevelBuilder.SetScene(sceneSp);
      mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
      mLuaLevelBuilder.RegisterCallbacks();
      mLuaLevelBuilder.RunScript();
      mLuaLevelBuilder.StopScript();
   }

   void MainMenuLevel::PostLevelInit()
   {
      Base::PostLevelInit();

#ifdef DEBUG
      if (EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig().EnableAmbientMusic)
      {
         if (const auto &sceneSp = mSceneWp.lock())
         {
            sceneSp->AddActor(mAmbientMusicDummy);
            mAmbientMusicDummy->SetScene(mSceneWp);
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

      mUiController->OnPostLevelInit();
   }

   void MainMenuLevel::PostPlayLevelFinished()
   {
      Base::PostPlayLevelFinished();
      mUiController->PostPlayLevelFinished();

#ifdef DEBUG
      if (EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig().EnableAmbientMusic)
      {
         mAmbientMusicDummy->GetComponentsByType<StreamingSoundComponent>().back()->PlayStream();
      }
#endif
   }

   void MainMenuLevel::InitLevel()
   {
      Base::InitLevel();
      CreateScene();
      mUiController->OnLevelInit();
   }

   void MainMenuLevel::UnloadLevel()
   {
      mUiController->CleanUp();
      mUiController.reset();
   }

   void MainMenuLevel::Tick(const float deltaTime)
   {
      if (mUiController)
      {
         mUiController->Tick(deltaTime);
      }
   }

   void MainMenuLevel::UnpausableTick(const float deltaTime)
   {
      if (mUiController)
      {
         mUiController->UnpausableTick(deltaTime);
      }
   }
}
