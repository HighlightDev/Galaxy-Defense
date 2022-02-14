#include "SpaceShipPlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{

   SpaceShipPlayerController::SpaceShipPlayerController(const std::shared_ptr<ACamera> playerCamera, std::shared_ptr<Actor> playerActor)
       : ActorController(playerActor), m_camera()
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

      std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();
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
            if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_FORWARD))
            {
               // movementComponent->Move();
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_LEFT))
            {
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_RIGHT))
            {
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_BACK))
            {
            }

            if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_JUMP))
            {
               // movementComponent->Jump();
            }
         }

         const std::vector<eKeyActionType> &currentFrameReleasedKeys = inputComponent->GetReleasedKeyActions();
         const std::vector<eKeyActionType> &currentFramePressedKeys = inputComponent->GetPressedKeyActions();

         // Buttons which have been released
         // if (currentFrameReleasedKeys.size())
         // {
         //    auto moveForwardIt = std::find(currentFrameReleasedKeys.begin(), currentFrameReleasedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
         //    if (moveForwardIt != currentFrameReleasedKeys.end())
         //    {
         //       m_playerActor->ChangeState("Idle");
         //    }
         // }

         // Buttons which have been pressed
         // if (currentFramePressedKeys.size())
         // {
         //    auto moveForwardIt = std::find(currentFramePressedKeys.begin(), currentFramePressedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
         //    if (moveForwardIt != currentFramePressedKeys.end())
         //    {
         //       m_playerActor->ChangeState("Walking");
         //    }
         // }
      }
   }

}