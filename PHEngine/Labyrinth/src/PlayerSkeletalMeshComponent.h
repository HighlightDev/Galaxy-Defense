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

      std::shared_ptr<AnimationPropertyBinding> mPropertiesBinding;

   public:

      PlayerSkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
         const SkeletalMeshRenderData& renderData);

      virtual ~PlayerSkeletalMeshComponent();

      virtual void Tick(float deltaTime) override;

      void ChangeState();

   private:

      void InitStateMachine();

   };
}