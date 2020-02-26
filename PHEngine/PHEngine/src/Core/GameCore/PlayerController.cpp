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

      if (m_playerActor->GetInputComponent())
      {
         auto& bindings = m_playerActor->GetInputComponent()->GetKeyboardBindings();
         std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();
         std::shared_ptr<MovementComponent> movementComponent = m_playerActor->GetMovementComponent();

         if (bindings.GetKeyState(Keys::W))
         {
            auto newPosition = movementComponent->GetMoveOffset() + rootComponent->GetTranslation();
            rootComponent->SetTranslation(newPosition);

            auto camera = movementComponent->GetCamera();
            if (ICamera::CameraType::THIRD_PERSON == camera->GetCameraType())
            {
               // TEST FAST VERSION
               auto myCamera = static_cast<ThirdPersonCamera*>(camera);
               if (myCamera->GetThirdPersonTarget() == m_playerActor)
               {
                  myCamera->SetThirdPersonTargetTransformationDirty();
               }
            }

            Event::PlayerMovedEvent::GetInstance()->SendEvent(newPosition);
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