#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"

#include <iostream>

namespace Game
{
   PhysicsComponent::PhysicsComponent(PhysicsDescriptor* descriptor)
      : mDescriptor(descriptor)
      , mTransformMatrix(glm::mat4(1))
      , bIsDirty(true)
   {
   }

   PhysicsComponent::~PhysicsComponent()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(mDescriptor->GetId());
   }

   void PhysicsComponent::Tick(const float deltaTime)
   {
      if (mDescriptor->GetMotionState())
      {
         float* physicsMatrix = mDescriptor->GetMotionWorldTransformMat4(this->bIsDirty);
         if (bIsDirty)
         {
            mTransformMatrix = GetWorldTransformMatrixFromArray(physicsMatrix);

            std::cout.clear();

            std::cout << "Dirty" << "matrix:" << std::endl << mTransformMatrix[0].x << " " << mTransformMatrix[0].y << mTransformMatrix[0].z << " " << mTransformMatrix[0].w << std::endl;
            std::cout << mTransformMatrix[1].x << " " << mTransformMatrix[1].y << mTransformMatrix[1].z << " " << mTransformMatrix[1].w << std::endl;
            std::cout << mTransformMatrix[2].x << " " << mTransformMatrix[2].y << mTransformMatrix[2].z << " " << mTransformMatrix[2].w << std::endl;
            std::cout << mTransformMatrix[3].x << " " << mTransformMatrix[3].y << mTransformMatrix[3].z << " " << mTransformMatrix[3].w << std::endl;
         }
      }
   }

   glm::mat4 PhysicsComponent::GetTransformMatrix() const
   {
      return mTransformMatrix;
   }

   bool PhysicsComponent::IsTransformDirty() const
   {
      return bIsDirty;
   }

   void PhysicsComponent::PostPhysicsInit()
   {
      const Actor* owner = GetOwner();
      const glm::vec3& translation = owner->GetRootComponent()->GetTranslation();
      const glm::vec3& rotation = owner->GetRootComponent()->GetRotation();

      mDescriptor->SetMotionStateWorldTransform(rotation.x, rotation.y, rotation.z, translation);

      mDescriptor->CompleteRigidBodyConstruction();
   }

   glm::mat4 PhysicsComponent::GetWorldTransformMatrixFromArray(float* mat) const
   {
      glm::mat4 result;
      for (int32_t i = 0, j = 0; i < 16; i += 4, ++j)
      {
         result[j].x = mat[i];
         result[j].y = mat[i + 1];
         result[j].z = mat[i + 2];
         result[j].w = mat[i + 3];
      }
      return result;
   }

   uint64_t PhysicsComponent::GetComponentType() const 
   {
      return PHYSICS_COMPONENT;
   }
}