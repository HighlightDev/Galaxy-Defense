#include "PlayerController.h"
#include "Core/GameCore/ICamera.h"
#include "COre/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsDescriptors/PhysicsDescriptor.h"

#include <tuple>

namespace Game
{

   PlayerController::PlayerController()
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
   }

   PlayerController::~PlayerController()
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
   }

   void PlayerController::SetPlayerActor(std::shared_ptr<Actor> playerActor)
   {
      m_playerActor = playerActor;

      if (auto rootComponent = m_playerActor->GetBaseRootComponent())
      {
         PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
      }
   }

   void PlayerController::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data)
   {
      std::string actorName = std::move(std::get<0>(data));

      if (m_playerActor && m_playerActor->GetName() == actorName)
      {
         if (auto rootComponent = m_playerActor->GetBaseRootComponent())
         {
            PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
         }
      }
   }

   void PlayerController::Tick(float deltaTime)
   {
      if (!m_playerActor)
         return;

      std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();
      std::shared_ptr<MovementComponent> movementComponent = m_playerActor->GetMovementComponent();

      if (movementComponent->GetIsCameraRotationDirty())
      {
         rootComponent->SetAdditionalRotation(movementComponent->GetCameraPitchYawRoll());
         movementComponent->SetIsCameraRotationDirty(false);
      }

      if (m_playerActor->GetInputComponent())
      {
         auto& bindings = m_playerActor->GetInputComponent()->GetKeyboardBindings();

         if (bindings.HasPressedKeys())
         {
            if (bindings.GetKeyState(Keys::W))
            {
               if (auto physComponent = m_playerActor->GetPhysicsComponent())
               {
                  glm::vec3 offset = movementComponent->GetMoveOffset();
                  physComponent->GetDescriptor()->SetLinearVelocity(btVector3(offset.x, 0, offset.z));
               }
           
            }
            else if (bindings.GetKeyState(Keys::A))
            {
            }
            else if (bindings.GetKeyState(Keys::D))
            {
            }
            else if (bindings.GetKeyState(Keys::S))
            {
               if (auto physComponent = m_playerActor->GetPhysicsComponent())
               {
                  glm::vec3 offset = movementComponent->GetMoveOffset();
                  physComponent->GetDescriptor()->SetLinearVelocity(btVector3(offset.x / 50.0f, 0, offset.z / 50.0f));
               }

            }
         }
   
      }

   }

}