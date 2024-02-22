#pragma once

#include "ActorController.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Components/InputComponent.h"

using namespace Event;

namespace EngineCore
{
   class ACamera;
   
   class HumanoidPlayerController
      : public ActorController
      , public PhysicsComponentUpdatedGameThreadEvent
   {

      using Base = Actor;

      std::shared_ptr<ACamera> m_camera;

      std::shared_ptr<InputComponent> m_inputComponent;

   public:

      HumanoidPlayerController(const std::shared_ptr<ACamera>& playerCamera, const std::shared_ptr<Actor>& actor);

      ~HumanoidPlayerController() override;

      void Initialize() override;

      void Tick(const float deltaTime) override;

      void ProcessEvent(const typename PhysicsComponentUpdatedGameThreadEvent::EventData_t& data) override;

   };

}

