#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

#include <TinyLogger/LogInterface.h>

using namespace EngineMath;
using namespace TinyLogger;

namespace EnginePhysics
{
   PhysicsComponent::PhysicsComponent(const std::shared_ptr<PhysicsComponentData> &data)
       : Component(data->EngineObjectName),
         mDescriptor(data->mPhysicsDescriptor),
         bIsTransformationDirty(true)
   {
      mDescriptor->SetOwnerComponentEngineObjectId(GetObjectId());
   }

   PhysicsComponent::~PhysicsComponent()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, mDescriptor->GetId());
   }

   void PhysicsComponent::CleanUp()
   {
   }

   void PhysicsComponent::SetOwner(const std::weak_ptr<Actor> &ownerActor)
   {
      LogInfo("PhysicsComponent::SetOwner");

      Component::SetOwner(ownerActor);
      const auto &ownerActorSp = ownerActor.lock();
      assert(ownerActorSp);

      mDescriptor->SetOwnerActorEngineObjectId(ownerActorSp->GetObjectId());
   }

   std::shared_ptr<PhysicsDescriptor> PhysicsComponent::GetDescriptor() const
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

   eComponentType PhysicsComponent::GetComponentType() const
   {
      return PHYSICS_COMPONENT;
   }

   void PhysicsComponent::PostPhysicsInit()
   {
      const auto &spOwner = GetOwner().lock();
      assert(spOwner);
      auto ownerRootComponent = spOwner->GetRootComponent();
      assert(ownerRootComponent);
      const glm::vec3 &translation = ownerRootComponent->GetTranslation();
      const glm::quat &rotator = ownerRootComponent->GetRotator();

      mDescriptor->SetMotionStateWorldTransform(Converter::glmToBullet(rotator), Converter::glmToBullet(translation));
      mDescriptor->CompletePhysicsDescriptorConstruction();
   }

   void PhysicsComponent::SetIsEnabled(const bool isEnabled)
   {
      Component::SetIsEnabled(isEnabled);
      mDescriptor->SetIsCollisionEnabled(isEnabled);
   }
}