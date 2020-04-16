#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include <glm/gtx/quaternion.hpp>

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

   glm::quat bulletToGlm(const btQuaternion& q)
   {
      return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
   }

   btQuaternion glmToBullet(const glm::quat& q) {
      return btQuaternion(q.x, q.y, q.z, q.w);
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

            owner->GetRootComponent()->SetTranslation(mDescriptor->GetTranslation(), true);
            //owner->GetRootComponent()->SetEulerRotationDegrees(mDescriptor->GetEulerRotationDegrees(), true);
            *owner->GetBaseRootComponent()->mRotator = bulletToGlm(mDescriptor->GetRotator());
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
      //const glm::vec3& rotation = owner->GetRootComponent()->GetEulerRotationDegrees();

      const glm::quat rotator = *owner->GetRootComponent()->mRotator;

      //const float yaw = DEG_TO_RAD(rotation.z), pitch = DEG_TO_RAD(rotation.y), roll = DEG_TO_RAD(rotation.x);

      //mDescriptor->SetMotionStateWorldTransform(yaw, pitch, roll, translation);
      mDescriptor->SetMotionStateWorldTransform(glmToBullet(rotator), translation);

      mDescriptor->CompleteRigidBodyConstruction();
   }

   uint64_t PhysicsComponent::GetComponentType() const 
   {
      return PHYSICS_COMPONENT;
   }
}