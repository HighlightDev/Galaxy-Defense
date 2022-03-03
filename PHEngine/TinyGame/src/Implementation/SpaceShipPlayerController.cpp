#include "SpaceShipPlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{

   SpaceShipPlayerController::SpaceShipPlayerController(const std::shared_ptr<ACamera> &playerCamera, const std::shared_ptr<Actor> &actor)
       : ActorController(actor), m_camera(), mCurrentState("")
   {
      assert((eCameraType::SECONDARY_FIRST_PERSON_CAMERA & playerCamera->GetCameraType()) == eCameraType::SECONDARY_FIRST_PERSON_CAMERA);
      m_camera = std::static_pointer_cast<FirstPersonCamera>(playerCamera);
   }

   SpaceShipPlayerController::~SpaceShipPlayerController()
   {
   }

   void SpaceShipPlayerController::InitActorController()
   {
      ActorController::InitActorController();

      const auto &rootComponent = m_actor->GetBaseRootComponent();

      assert(rootComponent);

      PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
   }

   void SpaceShipPlayerController::Tick(float deltaTime)
   {
      assert(m_actor);

      if ("" == mCurrentState)
      {
         mCurrentState = m_actor->GetTweener()->GetCurrentState()->GetStateName();
      }

      if (m_actor->GetInputComponent())
      {
         bool bMoveCommitted = true;
         const auto &inputComponent = m_actor->GetInputComponent();

         auto &mouseBindings = inputComponent->GetMouseBindings();

         if (mouseBindings.IsMouseScrollEventDirty())
         {
            const auto scrollDirection = mouseBindings.FlushMouseScrollEvent();
            m_camera->Zoom(scrollDirection, 1.25f);
         }

         const auto &keyboardBindings = inputComponent->GetKeyboardBindings();
         if (keyboardBindings.HasPressedKeys())
         {

            glm::vec3 direction(0.0f);

            if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_FORWARD))
            {
               direction.z = 1.0f;

               if ("s_fly_forward" != mCurrentState)
               {
                  mCurrentState = "s_fly_forward";
                  m_actor->ChangeState("s_fly_forward");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_LEFT))
            {
               direction.x = 1.0f;

               if ("s_fly_left" != mCurrentState)
               {
                  mCurrentState = "s_fly_left";
                  m_actor->ChangeState("s_fly_left");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_RIGHT))
            {
               direction.x = -1.0f;

               if ("s_fly_right" != mCurrentState)
               {
                  mCurrentState = "s_fly_right";
                  m_actor->ChangeState("s_fly_right");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_BACK))
            {
               direction.z = -1.0f;

               if ("s_fly_back" != mCurrentState)
               {
                  mCurrentState = "s_fly_back";
                  m_actor->ChangeState("s_fly_back");
               }
            }
            else
            {
               bMoveCommitted = false;
            }

            if (bMoveCommitted)
            {
               m_movementComponent->SetDirection(direction);
               m_movementComponent->Move();
            }
         }
         else
         {
            bMoveCommitted = false;

            if ("s_idle" != mCurrentState)
            {
               mCurrentState = "s_idle";
               m_actor->ChangeState("s_idle");
            }
         }

         if (bMoveCommitted || m_actor->GetTweener()->IsTransitionActive())
         {
            const auto &rootComponent = m_actor->GetBaseRootComponent();
            PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
         }
      }
   }
}