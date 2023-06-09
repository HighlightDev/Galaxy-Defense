#pragma once

#include "Core/GameCore/Level.h"
#include "Implementation/Controllers/SceneController.h"

using namespace EngineCore;

namespace Game
{

   class IntroLevel : public Level
   {
      using Base = Level;

      std::shared_ptr<SceneController> mSceneController;

   public:
      IntroLevel();

      ~IntroLevel() override;

      void InitLevel() override;

      void PreLevelInit() override;

      void PostLevelInit() override;

      void PostPlayLevelFinished() override;

      void RunLuaBuildLevelScript();

   private:
      void CreateScene();
   };

}
