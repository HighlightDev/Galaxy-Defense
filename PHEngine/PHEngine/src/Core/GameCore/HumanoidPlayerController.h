#pragma once

#include "ActorController.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Components/InputComponent.h"

using namespace Event;

namespace EngineCore
{
   class ACamera;
   
   class HumanoidPlayerController
      : public ActorController
      , public PhysicsSimulationUpdatedEvent
   {

      using Base = Actor;

      std::shared_ptr<ACamera> m_camera;

      std::shared_ptr<InputComponent> m_inputComponent;

   public:

      HumanoidPlayerController(const std::shared_ptr<ACamera>& playerCamera, const std::shared_ptr<Actor>& actor);

      virtual ~HumanoidPlayerController();

      virtual void Tick(float deltaTime) override;

      virtual void ProcessEvent(const typename PhysicsSimulationUpdatedEvent::EventData_t& data) override;

      virtual void InitActorController() override;
   };

}

