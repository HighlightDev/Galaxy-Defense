#pragma once
#include "Core/GameCore/Actor.h"

using namespace Game;

namespace Labyrinth
{
   class PlayerActor :
      public Actor
   {

   public:

      PlayerActor(const std::string& name, std::shared_ptr<Game::SceneComponent> rootComponent);

      virtual void PostConstructorInitialize() override;

   private:

      void InitStateMachine();

   };

}

