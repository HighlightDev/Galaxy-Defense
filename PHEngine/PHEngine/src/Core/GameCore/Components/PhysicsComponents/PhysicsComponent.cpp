#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"

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
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(mDescriptor->GetId());
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

            owner->GetRootComponent()->SetTranslation(mDescriptor->GetTranslation(), false);
            owner->GetRootComponent()->SetEulerRotationDegrees(mDescriptor->GetEulerRotationDegrees(), true);
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
      const glm::vec3& rotation = owner->GetRootComponent()->GetEulerRotationDegrees();

      const float yaw = DEG_TO_RAD(rotation.z), pitch = DEG_TO_RAD(rotation.y), roll = DEG_TO_RAD(rotation.x);

      mDescriptor->SetMotionStateWorldTransform(yaw, pitch, roll, translation);

      mDescriptor->CompleteRigidBodyConstruction();
   }

   uint64_t PhysicsComponent::GetComponentType() const 
   {
      return PHYSICS_COMPONENT;
   }
}