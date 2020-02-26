#pragma once
#include "Core/GameCore/Level.h"

using namespace Game;

class SimpleLevel :
   public Level
{
   SimpleLevel(InterThreadCommunicationMgr& interThreadCommunicationMgr);

   virtual void LoadLevel();
};

