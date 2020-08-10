#pragma once

#include "Actor.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Event/KeyboradInputEvent.h"

using namespace Event;
using namespace EnginePhysics;

namespace Game
{

   class PlayerController
      : public PhysicsSimulationUpdatedEvent
      , public KeyboardInputEvent
   {

      using Base = Actor;

      std::shared_ptr<Actor> m_playerActor;

      std::shared_ptr<CharacterPhysicsComponent> m_playerPhysicsComponent;

   public:

      PlayerController();

      virtual ~PlayerController();
      
      void SetPlayerActor(std::shared_ptr<Actor> playerActor);

      void Tick(float deltaTime);

      virtual void ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data) override;
      virtual void ProcessEvent(const KeyboardInputEvent::EventData_t& data) override;
   };

}

