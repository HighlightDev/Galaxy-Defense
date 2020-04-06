#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"

namespace Game
{
   PhysicsComponent::PhysicsComponent(PhysicsDescriptor* descriptor)
      : mDescriptor(descriptor)
   {
   }

   PhysicsComponent::~PhysicsComponent()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(mDescriptor->GetId());
   }

   void PhysicsComponent::Tick(const float deltaTime)
   {

   }

   void PhysicsComponent::PostPhysicsInit()
   {
      const Actor* owner = GetOwner();
      const glm::vec3& translation = owner->GetRootComponent()->GetTranslation();
      const glm::vec3& rotation = owner->GetRootComponent()->GetRotation();

      mDescriptor->SetMotionStateWorldTransform(rotation.x, rotation.y, rotation.z, translation);
   }
}