#pragma once

#include "Core/GameCore/Level.h"

using namespace Game;

namespace Labyrinth
{

   class SimpleLevel :
      public Level
   {
   public:

      SimpleLevel(InterThreadCommunicationMgr& threadMgr);

      virtual ~SimpleLevel();

      virtual void LoadLevel() override;
   };

}

