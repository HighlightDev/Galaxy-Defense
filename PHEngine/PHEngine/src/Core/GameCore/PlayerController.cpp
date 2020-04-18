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
      std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();

      if (rootComponent)
      {
         PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
      }
   }

   void PlayerController::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data)
   {
      std::string actorName = std::move(std::get<0>(data));

      if (m_playerActor->GetName() == actorName)
      {
         std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();

         if (rootComponent)
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

      if (auto physComponent = m_playerActor->GetPhysicsComponent())
      {
         physComponent->GetDescriptor()->GetRigidBody()->setAngularFactor(btVector3(0, 0, 0));
      }

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
                  if (!physComponent->GetDescriptor()->GetRigidBody()->isActive())
                  {
                     physComponent->GetDescriptor()->GetRigidBody()->activate();
                  }

                  glm::vec3 offset = movementComponent->GetMoveOffset();
                  physComponent->GetDescriptor()->GetRigidBody()->setLinearVelocity(btVector3(offset.x, 0, offset.z));
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
                  physComponent->GetDescriptor()->GetRigidBody()->setAngularFactor(btVector3(0, 0, 0));
                  if (!physComponent->GetDescriptor()->GetRigidBody()->isActive())
                  {
                     physComponent->GetDescriptor()->GetRigidBody()->activate();
                  }

                  glm::vec3 offset = movementComponent->GetMoveOffset();
                  physComponent->GetDescriptor()->GetRigidBody()->setLinearVelocity(btVector3(offset.x / 100, 0, offset.z / 100));
               }

            }
         }
   
      }

   }

}