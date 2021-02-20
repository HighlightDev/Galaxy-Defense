#pragma once

#include "Actor.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"

using namespace Event;
using namespace EnginePhysics;

namespace Game
{

   class PlayerController
      : public PhysicsSimulationUpdatedEvent
      , public KeyboardButtonDownEvent
   {

      using Base = Actor;

      std::shared_ptr<Actor> m_playerActor;

      std::shared_ptr<CharacterPhysicsComponent> m_playerPhysicsComponent;

   public:

      PlayerController(std::shared_ptr<Actor> playerActor);

      virtual ~PlayerController();

      void Tick(float deltaTime);

      std::shared_ptr<Actor> GetBindedActor() const;

      virtual void ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data) override;
      virtual void ProcessEvent(const KeyboardButtonDownEvent::EventData_t& data) override;

   private:

      void SetPlayerActor(std::shared_ptr<Actor> playerActor);
   };

}

