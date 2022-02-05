#pragma once

#include "ActorController.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"

using namespace Event;

namespace Game
{
   class ACamera;
   
   class PlayerController
      : public ActorController
      , public PhysicsSimulationUpdatedEvent
   {

      using Base = Actor;

      std::shared_ptr<ACamera> m_camera;

   public:

      PlayerController(std::shared_ptr<ACamera> playerCamera, std::shared_ptr<Actor> playerActor);

      virtual ~PlayerController();

      virtual void Tick(float deltaTime) override;

      virtual void ProcessEvent(const typename PhysicsSimulationUpdatedEvent::EventData_t& data) override;

   protected:

      virtual void SetPlayerActor(std::shared_ptr<Actor> playerActor) override;
   };

}

