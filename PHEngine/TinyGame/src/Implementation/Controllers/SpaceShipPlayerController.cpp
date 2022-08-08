#include "SpaceShipPlayerController.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/CommonCore/Assertion.h"
#include "Implementation/Events/MainPlayerActionEvent.h"

#include <algorithm>
#include <iostream>

namespace Game
{

   SpaceShipPlayerController::SpaceShipPlayerController(const std::shared_ptr<ACamera> &playerCamera, const std::shared_ptr<Actor> &actor)
       : ActorController(actor),
         m_camera(),
         mCurrentState(""),
         mLevelBounds(),
         mSpaceShipPrimitiveComponent()
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
      PlayerMovedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());

      const auto &primitiveComponents = m_actor->GetComponentsByType<PrimitiveComponent>();
      assert(primitiveComponents.size());

      const auto maxBoundPrimitiveComponentIt = std::max_element(primitiveComponents.begin(),
                                                                 primitiveComponents.end(),
                                                                 [&](const auto &leftPrimitiveComp, const auto &rightPrimitiveComp)
                                                                 {
                                                                    const auto &leftHalfExtent = leftPrimitiveComp->GetTransformedBoundingBox().GetHalfExtent();
                                                                    const auto &rightHalfExtent = rightPrimitiveComp->GetTransformedBoundingBox().GetHalfExtent();
                                                                    return glm::length2(leftHalfExtent) < glm::length2(rightHalfExtent);
                                                                 });

      assert(primitiveComponents.end() != maxBoundPrimitiveComponentIt);
      mSpaceShipPrimitiveComponent = *maxBoundPrimitiveComponentIt;
   }

   void SpaceShipPlayerController::SetLevelBounds(const BoundingBox &levelBounds)
   {
      mLevelBounds = levelBounds;
   }

   std::shared_ptr<FirstPersonCamera> SpaceShipPlayerController::GetCamera() const
   {
      return m_camera;
   }

   void SpaceShipPlayerController::Tick(float deltaTime)
   {
      assert(m_actor);

      if (!m_actor->IsEnabled())
         return;

      auto spaceshipTweener = m_actor->GetTweenerByName("SpaceshipMovement");
      assert(spaceshipTweener);

      if ("" == mCurrentState)
      {
         mCurrentState = spaceshipTweener->GetCurrentState()->GetStateName();
      }

      if (m_actor->GetInputComponent())
      {
         bool bMoveCommitted = true;
         const auto &inputComponent = m_actor->GetInputComponent();

         auto &mouseBindings = inputComponent->GetMouseBindings();

         if (mouseBindings.IsMouseScrollEventDirty())
         {
            const auto scrollDirection = mouseBindings.FlushMouseScrollEvent();
            m_camera->Zoom(scrollDirection, 4.0f);
         }

         if (KeyState::PRESSED == mouseBindings.GetKeyState(EngineCore::eMouseKeys::MouseButtonLeft))
         {
            MainPlayerActionEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, eMainPlayerActionEnum::SHOOT);
         }

         const auto &keyboardBindings = inputComponent->GetKeyboardBindings();
         if (keyboardBindings.HasPressedKeys())
         {
            const auto &levelAreaMinPos = mLevelBounds.GetMin();
            const auto &levelAreaMaxPos = mLevelBounds.GetMax();

            const auto &transformedBoundBox = mSpaceShipPrimitiveComponent->GetTransformedBoundingBox();
            const auto &shipMaxPos = transformedBoundBox.GetMax();
            const auto &shipMinPos = transformedBoundBox.GetMin();

            glm::vec3 direction(0.0f);

            if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_FORWARD))
            {
               if (shipMaxPos.z <= levelAreaMaxPos.z)
               {
                  direction.z = 1.0f;
               }

               if ("s_fly_forward" != mCurrentState)
               {
                  mCurrentState = "s_fly_forward";
                  m_actor->ChangeTweenerState("SpaceshipMovement", "s_fly_forward");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_LEFT))
            {
               if (shipMaxPos.x <= levelAreaMaxPos.x)
               {
                  direction.x = 1.0f;
               }

               if ("s_fly_left" != mCurrentState)
               {
                  mCurrentState = "s_fly_left";
                  m_actor->ChangeTweenerState("SpaceshipMovement", "s_fly_left");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_RIGHT))
            {
               if (shipMinPos.x >= levelAreaMinPos.x)
               {
                  direction.x = -1.0f;
               }

               if ("s_fly_right" != mCurrentState)
               {
                  mCurrentState = "s_fly_right";
                  m_actor->ChangeTweenerState("SpaceshipMovement", "s_fly_right");
               }
            }
            else if (KeyState::PRESSED == keyboardBindings.GetKeyState(eKeyActionType::ACTION_MOVE_BACK))
            {
               if (shipMinPos.z >= (levelAreaMinPos.z + 25.0f))
               {
                  direction.z = -1.0f;
               }

               if ("s_fly_back" != mCurrentState)
               {
                  mCurrentState = "s_fly_back";
                  m_actor->ChangeTweenerState("SpaceshipMovement", "s_fly_back");
               }
            }
            else
            {
               bMoveCommitted = false;
            }

            if (bMoveCommitted)
            {
               static constexpr float moveSpeed = 2000.0f;
               m_movementComponent->SetDirection(direction);
               m_movementComponent->Move(deltaTime * moveSpeed);
            }
         }
         else
         {
            bMoveCommitted = false;
         }

         if (!bMoveCommitted)
         {
            if ("s_idle" != mCurrentState)
            {
               mCurrentState = "s_idle";
               m_actor->ChangeTweenerState("SpaceshipMovement", "s_idle");
            }
         }

         if (bMoveCommitted || spaceshipTweener->IsTransitionActive())
         {
            const auto &rootComponent = m_actor->GetBaseRootComponent();
            PlayerMovedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
         }
      }
   }
}