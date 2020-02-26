#pragma once

#include "Actor.h"

namespace Game
{

   class PlayerController
   {

      using Base = Actor;

      std::shared_ptr<Actor> m_playerActor;

   public:

      PlayerController();

      virtual ~PlayerController();
      
      void SetPlayerActor(std::shared_ptr<Actor> playerActor);

      void Tick(float deltaTime);
   };

}

