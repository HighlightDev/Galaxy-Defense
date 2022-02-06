#include "PlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{

   PlayerController::PlayerController(const std::shared_ptr<ACamera> playerCamera, std::shared_ptr<Actor> playerActor)
       : ActorController(playerActor), m_camera(playerCamera)
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);

      SetPlayerActor(m_playerActor);
   }

   PlayerController::~PlayerController()
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
   }

   void PlayerController::SetPlayerActor(std::shared_ptr<Actor> playerActor)
   {
      assert(m_playerActor);

      const auto &rootComponent = m_playerActor->GetBaseRootComponent();

      assert(rootComponent);

      PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
   }

   void PlayerController::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t &data)
   {
      const std::string& actorName = std::move(std::get<0>(data));

      assert(m_playerActor);

      if (m_playerActor->GetName() == actorName)
      {
         if (auto rootComponent = m_playerActor->GetBaseRootComponent())
         {
            PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, rootComponent->GetTransformWeakPtr());
         }
      }
   }

   void PlayerController::Tick(float deltaTime)
   {
      assert(m_playerActor);

      std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();
      const auto& movementComponent = m_playerActor->GetMovementComponent();

      if (m_playerActor->GetInputComponent())
      {
         const auto &inputComponent = m_playerActor->GetInputComponent();

         auto &mouseBindings = inputComponent->GetMouseBindings();
         if (mouseBindings.IsMouseMoveEventDirty())
         {
            const auto &mouseMoveQueue = mouseBindings.FlushMouseMoveEvent();
            m_camera->SetRotation(mouseMoveQueue.z, mouseMoveQueue.w);
         }

         const auto &keyboardBindings = inputComponent->GetKeyboardBindings();
         if (keyboardBindings.HasPressedKeys())
         {
            if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_FORWARD))
            {
               movementComponent->Move();
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
               movementComponent->Jump();
            }
         }

         const std::vector<eKeyActionType> &currentFrameReleasedKeys = inputComponent->GetReleasedKeyActions();
         const std::vector<eKeyActionType> &currentFramePressedKeys = inputComponent->GetPressedKeyActions();

         // Buttons which have been released
         if (currentFrameReleasedKeys.size())
         {
            auto moveForwardIt = std::find(currentFrameReleasedKeys.begin(), currentFrameReleasedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
            if (moveForwardIt != currentFrameReleasedKeys.end())
            {
               m_playerActor->ChangeState("Idle");
            }
         }

         // Buttons which have been pressed
         if (currentFramePressedKeys.size())
         {
            auto moveForwardIt = std::find(currentFramePressedKeys.begin(), currentFramePressedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
            if (moveForwardIt != currentFramePressedKeys.end())
            {
               m_playerActor->ChangeState("Walking");
            }
         }
      }
   }

}