#include "PlayerController.h"
#include "Core/GameCore/ICamera.h"
#include "COre/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsDescriptors/PhysicsDescriptor.h"

namespace Game
{

   PlayerController::PlayerController()
   {
   }

   PlayerController::~PlayerController()
   {
   }

   void PlayerController::SetPlayerActor(std::shared_ptr<Actor> playerActor)
   {
      m_playerActor = playerActor;
      std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();

      if (rootComponent)
      {
         Event::PlayerMovedEvent::GetInstance()->SendEvent(rootComponent->GetTranslation());
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
         auto rotation = rootComponent->GetEulerRotationDegrees();
         rootComponent->SetEulerRotationDegrees(glm::vec3(0, rotation.y, 0));
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
                  physComponent->GetDescriptor()->GetRigidBody()->setAngularFactor(btVector3(0, 0, 0));
                  if (!physComponent->GetDescriptor()->GetRigidBody()->isActive())
                  {
                     physComponent->GetDescriptor()->GetRigidBody()->activate();
                  }

                  glm::vec3 offset = movementComponent->GetMoveOffset();
                  physComponent->GetDescriptor()->GetRigidBody()->setLinearVelocity(btVector3(offset.x, 0, offset.z));
               }

               //auto newPosition = movementComponent->GetMoveOffset() + rootComponent->GetTranslation();
               //rootComponent->SetTranslation(newPosition);

               Event::PlayerMovedEvent::GetInstance()->SendEvent(rootComponent->GetTranslation());
            }
            else if (bindings.GetKeyState(Keys::A))
            {
            }
            else if (bindings.GetKeyState(Keys::D))
            {
            }
            else if (bindings.GetKeyState(Keys::S))
            {
            }
         }
   
      }

   }

}