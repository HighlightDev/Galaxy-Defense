#include "PlayerController.h"
#include "Core/GameCore/ICamera.h"
#include "COre/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"

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
               auto newPosition = movementComponent->GetMoveOffset() + rootComponent->GetTranslation();
               rootComponent->SetTranslation(newPosition);

               Event::PlayerMovedEvent::GetInstance()->SendEvent(newPosition);
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
         }
      }

   }

}