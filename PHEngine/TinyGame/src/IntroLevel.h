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
      IntroLevel(InterThreadCommunicationMgr &threadMgr);

      virtual ~IntroLevel();

      virtual void InitLevel() override;

      virtual void PreLevelInit();

      virtual void PostLevelInit();

      virtual void PostPlayLevelFinished() override;

      void RunLuaBuildLevelScript();

   private:
      void CreateScene();
   };

}
