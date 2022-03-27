#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

using namespace EngineMath;

namespace EnginePhysics
{
   PhysicsComponent::PhysicsComponent(const PhysicsComponentData& data)
      : Component(data.GameObjectName)
      , mDescriptor(data.mPhysicsDescriptor)
      , bIsTransformationDirty(true)
   {
   }

   PhysicsComponent::~PhysicsComponent()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, mDescriptor->GetId());
   }

   PhysicsDescriptor* PhysicsComponent::GetDescriptor() const {
      return mDescriptor;
   }

   void PhysicsComponent::Tick(const float deltaTime)
   {
      Component::Tick(deltaTime);

      if (mDescriptor->GetMotionState())
      {
         bool bIsDirty;

         mDescriptor->UpdateMotionWorldTransformLocalState(bIsDirty, deltaTime);

         bIsTransformationDirty = bIsDirty;

         if (bIsDirty)
         {
            if (const auto& spOwner = GetOwner().lock())
            {
               spOwner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(mDescriptor->GetTranslation()));
               spOwner->GetRootComponent()->SetRotator(Converter::bulletToGlm(mDescriptor->GetRotator()));

               Event::PhysicsSimulationUpdatedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::PRE_EXECUTION, spOwner->GetName());
            }
         }
      }
   }

   void PhysicsComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      SerializeDataActor& actorData = Component::GetSerializeDataActor(dataContainer);
      auto physCompData = SerializeHelper::GetSerializeDataPhysicsComponent(this);
      physCompData->ComponentName = GameObjectName;

      actorData.ComponentsData.emplace_back(physCompData);
   }

   bool PhysicsComponent::IsTransformDirty() const
   {
      return bIsTransformationDirty;
   }

   void PhysicsComponent::PostPhysicsInit()
   {
      const auto& spOwner = GetOwner().lock();
      assert(spOwner);

      const glm::vec3& translation = spOwner->GetRootComponent()->GetTranslation();
      const glm::quat& rotator = spOwner->GetRootComponent()->GetRotator();

      mDescriptor->SetMotionStateWorldTransform(Converter::glmToBullet(rotator), Converter::glmToBullet(translation));
      mDescriptor->CompletePhysicsDescriptorConstruction();
   }

   glm::vec3 PhysicsComponent::GetWorldTranslation() const
   {
      return Converter::bulletToGlm(mDescriptor->GetTranslation());
   }

   glm::quat PhysicsComponent::GetWorldRotator() const
   {
      return Converter::bulletToGlm(mDescriptor->GetRotator());
   }

   void PhysicsComponent::SetWorldTranslation(const glm::vec3& translation) const
   {
       mDescriptor->SetTranslation(Converter::glmToBullet(translation));
   }

   void PhysicsComponent::SetWorldRotator(const glm::quat& rotator) const
   {
      mDescriptor->SetRotator(Converter::glmToBullet(rotator));
   }

   ComponentType PhysicsComponent::GetComponentType() const
   {
      return PHYSICS_COMPONENT;
   }
}