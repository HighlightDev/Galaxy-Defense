#include "HumanoidPlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   HumanoidPlayerController::HumanoidPlayerController(const std::shared_ptr<ACamera>& playerCamera, const std::shared_ptr<Actor>& actor)
       : ActorController(actor), m_camera(playerCamera), m_inputComponent()
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
   }

   HumanoidPlayerController::~HumanoidPlayerController()
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
   }

   void HumanoidPlayerController::InitActorController()
   {
      ActorController::InitActorController();

      const auto &rootComponent = m_actor->GetBaseRootComponent();
      assert(rootComponent);

      m_inputComponent = m_actor->GetInputComponent();
      assert(m_inputComponent);

      PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
   }

   void HumanoidPlayerController::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t &data)
   {
      const std::string& actorName = std::move(std::get<0>(data));

      assert(m_actor);

      if (m_actor->GetName() == actorName)
      {
         if (auto rootComponent = m_actor->GetBaseRootComponent())
         {
            PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, rootComponent->GetTransformWeakPtr());
         }
      }
   }

   void HumanoidPlayerController::Tick(float deltaTime)
   {
      assert(m_actor);

      std::shared_ptr<SceneComponent> rootComponent = m_actor->GetBaseRootComponent();

      if (m_actor->GetInputComponent())
      {
         const auto &inputComponent = m_actor->GetInputComponent();

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
               m_movementComponent->Move();
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
               m_movementComponent->Jump();
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
               m_actor->ChangeTweenState("Idle");
            }
         }

         // Buttons which have been pressed
         if (currentFramePressedKeys.size())
         {
            auto moveForwardIt = std::find(currentFramePressedKeys.begin(), currentFramePressedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
            if (moveForwardIt != currentFramePressedKeys.end())
            {
               m_actor->ChangeTweenState("Walking");
            }
         }
      }
   }

}