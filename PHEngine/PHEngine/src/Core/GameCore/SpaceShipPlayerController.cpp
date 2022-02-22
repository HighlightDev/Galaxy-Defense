#include "SpaceShipPlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   SpaceShipPlayerController::SpaceShipPlayerController(const std::shared_ptr<ACamera> playerCamera, std::shared_ptr<Actor> playerActor)
       : ActorController(playerActor), m_camera(), mCurrentState("")
   {
      assert((eCameraType::SECONDARY_THIRD_PERSON_CAMERA & playerCamera->GetCameraType()) == eCameraType::SECONDARY_THIRD_PERSON_CAMERA);
      m_camera = std::static_pointer_cast<ThirdPersonCamera>(playerCamera);
      InitPlayerController();
   }

   SpaceShipPlayerController::~SpaceShipPlayerController()
   {
   }

   void SpaceShipPlayerController::InitPlayerController()
   {
      assert(m_playerActor);

      const auto &rootComponent = m_playerActor->GetBaseRootComponent();

      assert(rootComponent);

      PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
   }

   void SpaceShipPlayerController::Tick(float deltaTime)
   {
      assert(m_playerActor);

      if ("" == mCurrentState)
      {
         mCurrentState = m_playerActor->GetTweener()->GetCurrentState()->GetStateName();
      }

      const auto &movementComponent = m_playerActor->GetMovementComponent();

      if (m_playerActor->GetInputComponent())
      {
         const auto &inputComponent = m_playerActor->GetInputComponent();

         auto &mouseBindings = inputComponent->GetMouseBindings();
         if (mouseBindings.IsMouseMoveEventDirty())
         {
            const auto &mouseMoveQueue = mouseBindings.FlushMouseMoveEvent();
            m_camera->SetRotation(mouseMoveQueue.z, mouseMoveQueue.w);
         }

         if (mouseBindings.IsMouseScrollEventDirty())
         {
            const auto scrollDirection = mouseBindings.FlushMouseScrollEvent();
            m_camera->Zoom(scrollDirection, 1.25f);
         }

         const auto &keyboardBindings = inputComponent->GetKeyboardBindings();
         if (keyboardBindings.HasPressedKeys())
         {
            bool bMoveCommitted = true;
            glm::vec3 direction(0.0f);

            if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_FORWARD))
            {
               direction.z = -1.0f;

               if ("s_fly_forward" != mCurrentState)
               {
                  mCurrentState = "s_fly_forward";
                  m_playerActor->ChangeState("s_fly_forward");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_LEFT))
            {
               direction.x = -1.0f;

               if ("s_fly_left" != mCurrentState)
               {
                  mCurrentState = "s_fly_left";
                  m_playerActor->ChangeState("s_fly_left");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_RIGHT))
            {
               direction.x = 1.0f;

               if ("s_fly_right" != mCurrentState)
               {
                  mCurrentState = "s_fly_right";
                  m_playerActor->ChangeState("s_fly_right");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_BACK))
            {
               direction.z = 1.0f;

               if ("s_fly_back" != mCurrentState)
               {
                  mCurrentState = "s_fly_back";
                  m_playerActor->ChangeState("s_fly_back");
               }
            }
            else
            {
               bMoveCommitted = false;
            }

            if (bMoveCommitted)
            {
               movementComponent->SetDirection(direction);
               movementComponent->Move();
               const auto &rootComponent = m_playerActor->GetBaseRootComponent();
               PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
            }
         }
         else
         {
            if ("s_idle" != mCurrentState)
            {
               mCurrentState = "s_idle";
               m_playerActor->ChangeState("s_idle");
            }
         }
      }
   }
}