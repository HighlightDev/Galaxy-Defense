#pragma once

#include "Core/GameCore/Level.h"

using namespace Game;

namespace Labyrinth
{

   class IntroLevel :
      public Level
   {
      using Base = Level;

   public:

      IntroLevel(InterThreadCommunicationMgr& threadMgr);

      virtual ~IntroLevel();

      virtual void LoadLevel() override;

      virtual void PreLevelInit();

      virtual void PostLevelInit();

      void RunLuaBuildLevelScript();

   private:

      void CreateScene();
   };

}

