#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

using namespace EngineMath;

namespace Game
{
   PhysicsComponent::PhysicsComponent(PhysicsDescriptor* descriptor)
      : mDescriptor(descriptor)
      , bIsTransformationDirty(true)
   {
   }

   PhysicsComponent::~PhysicsComponent()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, mDescriptor->GetId());
   }

   PhysicsDescriptor* PhysicsComponent::GetDescriptor() {
      return mDescriptor;
   }

   void PhysicsComponent::Tick(const float deltaTime)
   {
      if (mDescriptor->GetMotionState())
      {
         bool bIsDirty;

         mDescriptor->UpdateMotionWorldTransformLocalState(bIsDirty);

         bIsTransformationDirty = bIsDirty;

         if (bIsDirty)
         {
            const Actor* owner = GetOwner();

            owner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(mDescriptor->GetTranslation()), false);
            owner->GetBaseRootComponent()->SetRotator(Converter::bulletToGlm(mDescriptor->GetRotator()), true);
         }
      }
   }

   bool PhysicsComponent::IsTransformDirty() const
   {
      return bIsTransformationDirty;
   }

   void PhysicsComponent::PostPhysicsInit()
   {
      const Actor* owner = GetOwner();
      const glm::vec3& translation = owner->GetRootComponent()->GetTranslation();
      const glm::quat& rotator = owner->GetRootComponent()->GetRotator();

      mDescriptor->SetMotionStateWorldTransform(Converter::glmToBullet(rotator), Converter::glmToBullet(translation));

      mDescriptor->CompleteRigidBodyConstruction();
   }

   uint64_t PhysicsComponent::GetComponentType() const 
   {
      return PHYSICS_COMPONENT;
   }
}