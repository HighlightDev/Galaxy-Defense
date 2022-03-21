#pragma once

#include "Core/GameCore/Level.h"
#include "Implementation/SceneController.h"

using namespace EngineCore;

namespace Game
{

   class IntroLevel : public Level
   {
      using Base = Level;

      std::shared_ptr<SceneController> mEnemySceneController;

   public:
      IntroLevel(InterThreadCommunicationMgr &threadMgr);

      virtual ~IntroLevel();

      virtual void LoadLevel() override;

      virtual void PreLevelInit();

      virtual void PostLevelInit();

      void RunLuaBuildLevelScript();

   private:
      void CreateScene();
   };

}
