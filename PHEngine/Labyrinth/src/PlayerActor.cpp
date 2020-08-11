#include "PlayerActor.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"

namespace Labyrinth
{

   PlayerActor::PlayerActor(const std::string& name, std::shared_ptr<Game::SceneComponent> rootComponent)
      : Actor(name, rootComponent)
      , mStateMachine(nullptr)
      , mPropertiesBinding(nullptr)
   {
   }

   PlayerActor::~PlayerActor()
   {
      delete mStateMachine;
   }

   void PlayerActor::PostConstructorInitialize()
   {
      Actor::PostConstructorInitialize();

      InitStateMachine();
   }

   void PlayerActor::ChangeState(const std::string& stateName)
   {
      mStateMachine->ChangeState(stateName);
   }

   void PlayerActor::Tick(const float deltaTime) 
   {
      Actor::Tick(deltaTime);

      mStateMachine->Tick(deltaTime);
   }

   void PlayerActor::InitStateMachine()
   {
      std::shared_ptr<SkeletalMeshComponent> comp = GetComponent<SkeletalMeshComponent>(SKELETAL_MESH_COMPONENT);

      //// Init reference binding
      mPropertiesBinding = std::make_shared<AnimationPropertyBinding>(comp->GetSrcAnimationNameRef(), comp->GetDstAnimationNameRef(),
         comp->GetSrcAnimationTimeRef(), comp->GetDstAnimationTimeRef(), comp->GetIsTransitionEnabledRef(), comp->GetTransitionValueRef());

      State* stateIdle = new State("State Idle");
      State* stateWalking = new State("State Walking");

      StateProperty<StatePropertyType::Animation>* prop_idleAnim = new StateProperty<StatePropertyType::Animation>("Iddle", mPropertiesBinding);
      stateIdle->AddStateProperty("Prop_AnimationTina", prop_idleAnim);

      StateTransition transitionFromIdleToWalking(stateIdle, stateWalking, 0.5f);
      stateIdle->AddStateTransition(transitionFromIdleToWalking);

      StateTransition transitionFromWalkingToIdle(stateWalking, stateIdle, 0.5f);
      stateWalking->AddStateTransition(transitionFromWalkingToIdle);

      StateProperty<StatePropertyType::Animation>* prop_walkAnim = new StateProperty<StatePropertyType::Animation>("Armature|Walk", mPropertiesBinding);
      stateWalking->AddStateProperty("Prop_AnimationTina", prop_walkAnim);

      mStateMachine = new StateMachine(stateIdle);
   }

}
