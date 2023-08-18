#include "HumanoidPlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   HumanoidPlayerController::HumanoidPlayerController(const std::shared_ptr<ACamera> &playerCamera, const std::shared_ptr<Actor> &actor)
       : ActorController(actor), m_camera(playerCamera), m_inputComponent()
   {
      PhysicsComponentUpdatedEvent::GetInstance()->AddListener(this);
   }

   HumanoidPlayerController::~HumanoidPlayerController()
   {
      PhysicsComponentUpdatedEvent::GetInstance()->RemoveListener(this);
   }

   void HumanoidPlayerController::InitActorController()
   {
      ActorController::InitActorController();

      const auto &actorSp = m_actorWp.lock();
      const auto &rootComponent = actorSp->GetBaseRootComponent();
      assert(rootComponent);

      m_inputComponent = actorSp->GetInputComponent();
      assert(m_inputComponent);

      PlayerMovedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
   }

   void HumanoidPlayerController::ProcessEvent(const PhysicsComponentUpdatedEvent::EventData_t &data)
   {
      const std::string &actorName = std::move(std::get<0>(data));

      if (const auto &actorSp = m_actorWp.lock())
      {
         if (actorSp->GetName() == actorName)
         {
            if (auto rootComponent = actorSp->GetBaseRootComponent())
            {
               PlayerMovedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, rootComponent->GetTransformWeakPtr());
            }
         }
      }
   }

   void HumanoidPlayerController::Tick(float deltaTime)
   {
      if (const auto &actorSp = m_actorWp.lock())
      {
         if (!actorSp->IsEnabled())
            return;

         const auto &rootComponent = actorSp->GetBaseRootComponent();

         if (actorSp->GetInputComponent())
         {
            const auto &inputComponent = actorSp->GetInputComponent();

            auto &mouseBindings = inputComponent->GetMouseBindings();
            if (mouseBindings.IsMouseMoveEventDirty())
            {
               const auto &mouseMoveQueue = mouseBindings.FlushMouseMoveEvent();
               m_camera->SetRotation(mouseMoveQueue.z, mouseMoveQueue.w);
            }

            const auto &keyboardBindings = inputComponent->GetKeyboardBindings();
            if (keyboardBindings.HasPressedKeys())
            {
               if (KeyState::PRESSED == keyboardBindings.GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_FORWARD))
               {
                  if (const auto &moveCompSp = m_movementComponentWp.lock())
                  {
                     moveCompSp->Move(deltaTime);
                  }
               }
               else if (KeyState::PRESSED == keyboardBindings.GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_LEFT))
               {
               }
               else if (KeyState::PRESSED == keyboardBindings.GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_RIGHT))
               {
               }
               else if (KeyState::PRESSED == keyboardBindings.GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_BACK))
               {
               }

               if (KeyState::PRESSED == keyboardBindings.GetKeyStateByActionType(eKeyActionType::ACTION_JUMP))
               {
                  if (const auto &moveCompSp = m_movementComponentWp.lock())
                  {
                     moveCompSp->Jump();
                  }
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
                  actorSp->ChangeTweenerState("CharacterAnimation", "Idle");
               }
            }

            // Buttons which have been pressed
            if (currentFramePressedKeys.size())
            {
               auto moveForwardIt = std::find(currentFramePressedKeys.begin(), currentFramePressedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
               if (moveForwardIt != currentFramePressedKeys.end())
               {
                  actorSp->ChangeTweenerState("CharacterAnimation", "Walking");
               }
            }
         }
      }
   }

}