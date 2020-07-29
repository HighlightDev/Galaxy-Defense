#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/StateMachine/StateMachine.h"

using namespace Game;

namespace Labyrinth
{
   class PlayerSkeletalMeshComponent
      : public SkeletalMeshComponent
   {
      using Base = SkeletalMeshComponent;

      StateMachine* mAnimationStateMachine;

   public:

      PlayerSkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
         const SkeletalMeshRenderData& renderData)
         : SkeletalMeshComponent(translation, rotation, scale, renderData)
      {

      }

      virtual ~PlayerSkeletalMeshComponent()
      {
         delete mAnimationStateMachine;
      }

      virtual void Tick(float deltaTime) override
      {
         Base::Tick(deltaTime);

         mAnimationStateMachine->Tick(deltaTime);

      }

   private:

      void InitStateMachine()
      {
         State* stateIdle = new State("State Idle");
         State* stateWalking = new State("State Walking");

         StateProperty<StatePropertyType::Animation>* prop_idleAnim = new StateProperty<StatePropertyType::Animation>("Idle");
         stateIdle->AddStateProperty("Prop_AnimationIdle", prop_idleAnim);

         StateTransition transitionFromIdleToWalking(stateIdle, stateWalking, 1.0f);
         stateIdle->AddStateTransition(transitionFromIdleToWalking);

         StateTransition transitionFromWalkingToIdle(stateWalking, stateIdle, 1.0f);
         stateWalking->AddStateTransition(transitionFromWalkingToIdle);

         StateProperty<StatePropertyType::Animation>* prop_walkAnim = new StateProperty<StatePropertyType::Animation>("Walk");
         stateWalking->AddStateProperty("Prop_AnimationWalk", prop_walkAnim);

         mAnimationStateMachine = new StateMachine(stateIdle);
      }

   };
}