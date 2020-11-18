#include "PlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "COre/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/CommonCore/Assertion.h"

#include <tuple>

namespace Game
{

   PlayerController::PlayerController(std::shared_ptr<Actor> playerActor)
      : m_playerPhysicsComponent()
      , m_playerActor(playerActor)
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
      KeyboardButtonDownEvent::GetInstance()->AddListener(this);
      SetPlayerActor(m_playerActor);
   }

   PlayerController::~PlayerController()
   {
      PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
      KeyboardButtonDownEvent::GetInstance()->RemoveListener(this);
   }

   void PlayerController::SetPlayerActor(std::shared_ptr<Actor> playerActor)
   {
      m_playerPhysicsComponent = std::static_pointer_cast<CharacterPhysicsComponent>(m_playerActor->GetPhysicsComponent());

      assert(m_playerActor);

      const auto& rootComponent = m_playerActor->GetBaseRootComponent();

      assert(rootComponent);
      assert(m_playerPhysicsComponent);

      PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
   }

   void PlayerController::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data)
   {
      std::string actorName = std::move(std::get<0>(data));

      assert(m_playerActor);

      if (m_playerActor->GetName() == actorName)
      {
         if (auto rootComponent = m_playerActor->GetBaseRootComponent())
         {
            PlayerMovedEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, rootComponent->GetTransformWeakPtr());
         }
      }
   }

   std::shared_ptr<Actor> PlayerController::GetBindedActor() const
   {
      return m_playerActor;
   }

   void PlayerController::ProcessEvent(const KeyboardButtonDownEvent::EventData_t& eventData)
   {
      assert(m_playerActor);
      const auto& data = std::get<0>(eventData);

      if (data.Key == Keys::W)
      {
         if (data.State == KeyState::PRESSED)
         {
            m_playerActor->ChangeState("Walking");
         }
         else
         {
            m_playerActor->ChangeState("Idle");
         }
      }
   }

   void PlayerController::Tick(float deltaTime)
   {
      assert(m_playerActor);

      std::shared_ptr<SceneComponent> rootComponent = m_playerActor->GetBaseRootComponent();
      std::shared_ptr<CharacterMovementComponent> movementComponent = m_playerActor->GetMovementComponent();

      if (movementComponent->GetIsCameraRotationDirty())
      {
         rootComponent->SetAdditionalRotation(movementComponent->GetCameraPitchYawRoll());
         movementComponent->SetIsCameraRotationDirty(false);
      }

      if (m_playerActor->GetInputComponent())
      {
         const auto& bindings = m_playerActor->GetInputComponent()->GetKeyboardBindings();

         if (bindings.HasPressedKeys())
         {
            if (KeyState::PRESSED == bindings.GetKeyState(Keys::W))
            {
               m_playerPhysicsComponent->SetWalkVelocity(movementComponent->GetVelocity());
            }
            else if (KeyState::PRESSED == bindings.GetKeyState(Keys::A))
            {
            }
            else if (KeyState::PRESSED == bindings.GetKeyState(Keys::D))
            {
            }
            else if (KeyState::PRESSED == bindings.GetKeyState(Keys::S))
            {
            }

            if (KeyState::PRESSED == bindings.GetKeyState(Keys::Space))
            {
               m_playerPhysicsComponent->SetJumpVelocity();
            }
         }
      }

   }

}