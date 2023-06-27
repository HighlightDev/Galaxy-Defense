#pragma once

#include "Core/GameCore/Level.h"
#include "Implementation/Controllers/MainMenuLevelUiController.h"

#include <memory>

using namespace EngineCore;

namespace Game
{

   class MainMenuLevel : public Level
   {
      using Base = Level;

      std::unique_ptr<MainMenuLevelUiController> mUiController;

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
