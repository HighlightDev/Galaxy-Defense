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

   #include <iostream>

   void SpaceShipPlayerController::Tick(float deltaTime)
   {
      assert(m_playerActor);

      static uint64_t i = 0;

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
               std::cout << "keyboardBindings.HasPressedKeys() i = " << i++<< std::endl;
               movementComponent->SetDirection(glm::vec3(0, 0, 1));
               movementComponent->Move();
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_LEFT))
            {
               movementComponent->SetDirection(glm::vec3(-1, 0, 0));
               movementComponent->Move();
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_RIGHT))
            {
               movementComponent->SetDirection(glm::vec3(1, 0, 0));
               movementComponent->Move();
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_BACK))
            {
               movementComponent->SetDirection(glm::vec3(0, 0, -1));
               movementComponent->Move();
            }

            if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_JUMP))
            {
               // movementComponent->Jump();
            }
         }
      }
   }

}