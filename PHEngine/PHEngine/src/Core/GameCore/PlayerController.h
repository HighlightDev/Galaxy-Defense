#pragma once

#include "Actor.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"

using namespace Event;

namespace Game
{

   class PlayerController
      : public PhysicsSimulationUpdatedEvent
   {

      using Base = Actor;

      std::shared_ptr<Actor> m_playerActor;

   public:

      PlayerController();

      virtual ~PlayerController();
      
      void SetPlayerActor(std::shared_ptr<Actor> playerActor);

      void Tick(float deltaTime);

      virtual void ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data) override;
   };

}

