#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

using namespace EngineMath;

namespace EnginePhysics
{
   PhysicsComponent::PhysicsComponent(const PhysicsComponentData &data)
       : Component(data.GameObjectName), mDescriptor(data.mPhysicsDescriptor), bIsTransformationDirty(true)
   {
   }

   PhysicsComponent::~PhysicsComponent()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, mDescriptor->GetId());
   }

   PhysicsDescriptor *PhysicsComponent::GetDescriptor() const
   {
      return mDescriptor;
   }

   bool PhysicsComponent::IsTransformDirty() const
   {
      return bIsTransformationDirty;
   }

   glm::vec3 PhysicsComponent::GetWorldTranslation() const
   {
      return Converter::bulletToGlm(mDescriptor->GetTranslation());
   }

   glm::quat PhysicsComponent::GetWorldRotator() const
   {
      return Converter::bulletToGlm(mDescriptor->GetRotator());
   }

   void PhysicsComponent::SetWorldTranslation(const glm::vec3 &translation) const
   {
      mDescriptor->SetTranslation(Converter::glmToBullet(translation));
   }

   void PhysicsComponent::SetWorldRotator(const glm::quat &rotator) const
   {
      mDescriptor->SetRotator(Converter::glmToBullet(rotator));
   }

   ComponentType PhysicsComponent::GetComponentType() const
   {
      return PHYSICS_COMPONENT;
   }

   void PhysicsComponent::PostPhysicsInit()
   {
      const auto &spOwner = GetOwner().lock();
      assert(spOwner);

      const glm::vec3 &translation = spOwner->GetRootComponent()->GetTranslation();
      const glm::quat &rotator = spOwner->GetRootComponent()->GetRotator();

      mDescriptor->SetMotionStateWorldTransform(Converter::glmToBullet(rotator), Converter::glmToBullet(translation));
      mDescriptor->CompletePhysicsDescriptorConstruction();
   }

   void PhysicsComponent::SetIsEnabled(const bool isEnabled)
   {
      Component::SetIsEnabled(isEnabled);
      mDescriptor->SetIsCollisionEnabled(isEnabled);
   }
}