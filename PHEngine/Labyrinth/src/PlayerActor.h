#pragma once
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/StateMachine/StateMachine.h"

using namespace Game;

namespace Labyrinth
{
   class PlayerActor :
      public Actor
   {

      StateMachine* mStateMachine;

      std::shared_ptr<AnimationPropertyBinding> mPropertiesBinding;

   public:

      PlayerActor(const std::string& name, std::shared_ptr<Game::SceneComponent> rootComponent);

      virtual ~PlayerActor();

      virtual void ChangeState(const std::string& stateName) override;

      virtual void Tick(const float deltaTime) override;

      virtual void PostConstructorInitialize() override;

   private:

      void InitStateMachine();

   };

}

