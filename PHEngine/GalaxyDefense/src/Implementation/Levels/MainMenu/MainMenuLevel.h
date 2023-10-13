#pragma once

#include "Implementation/Levels/LevelBase.h"
#include "Implementation/Levels/MainMenu/Controllers/MainMenuLevelUiController.h"

#include <memory>

using namespace EngineCore;

namespace EngineCore
{
   class Actor;
}

namespace Game
{

   class MainMenuLevel : public LevelBase
   {
      using Base = LevelBase;

      std::unique_ptr<MainMenuLevelUiController> mUiController;

      std::shared_ptr<Actor> mAmbientMusicDummy;

   public:
      MainMenuLevel();

      ~MainMenuLevel() override;

      void InitLevel() override;

      void PreLevelInit() override;

      void PostLevelInit() override;

      void PostPlayLevelFinished() override;

      void UnloadLevel() override;

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override;

   private:
      void CreateScene();

      void RunLuaBuildLevelScript();
   };

}
