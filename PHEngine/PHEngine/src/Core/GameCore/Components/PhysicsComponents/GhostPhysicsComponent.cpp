#include "GhostPhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

using namespace EngineMath;

namespace EnginePhysics
{
   GhostPhysicsComponent::GhostPhysicsComponent(const PhysicsComponentData &data)
       : Component(data.GameObjectName), mDescriptor(data.mPhysicsDescriptor), bIsTransformationDirty(true)
   {
   }

   GhostPhysicsComponent::~GhostPhysicsComponent()
   {
      Event::PhysicsDescriptorRemovedEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, mDescriptor->GetId());
   }

   PhysicsDescriptor *GhostPhysicsComponent::GetDescriptor() const
   {
      return mDescriptor;
   }

   void GhostPhysicsComponent::Tick(const float deltaTime)
   {
      Component::Tick(deltaTime);

      if (const auto &spOwner = GetOwner().lock())
      {
         const glm::vec3 &translation = spOwner->GetRootComponent()->GetTranslation();
         const glm::quat &rotator = spOwner->GetRootComponent()->GetRotator();
         mDescriptor->SetMotionStateWorldTransform(Converter::glmToBullet(rotator), Converter::glmToBullet(translation));
      }

      bool bIsDirty;

      mDescriptor->UpdateMotionWorldTransformLocalState(bIsDirty, deltaTime);
      bIsTransformationDirty = bIsDirty;
   }

   void GhostPhysicsComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      // SerializeDataActor &actorData = Component::GetSerializeDataActor(dataContainer);
      // auto physCompData = SerializeHelper::GetSerializeDataPhysicsComponent(this);
      // physCompData->ComponentName = GameObjectName;

      // actorData.ComponentsData.emplace_back(physCompData);
   }

   bool GhostPhysicsComponent::IsTransformDirty() const
   {
      return bIsTransformationDirty;
   }

   void GhostPhysicsComponent::PostPhysicsInit()
   {
      const auto &spOwner = GetOwner().lock();
      assert(spOwner);

      const glm::vec3 &translation = spOwner->GetRootComponent()->GetTranslation();
      const glm::quat &rotator = spOwner->GetRootComponent()->GetRotator();

      mDescriptor->SetMotionStateWorldTransform(Converter::glmToBullet(rotator), Converter::glmToBullet(translation));
      mDescriptor->CompletePhysicsDescriptorConstruction();
   }

   glm::vec3 GhostPhysicsComponent::GetWorldTranslation() const
   {
      return Converter::bulletToGlm(mDescriptor->GetTranslation());
   }

   glm::quat GhostPhysicsComponent::GetWorldRotator() const
   {
      return Converter::bulletToGlm(mDescriptor->GetRotator());
   }

   void GhostPhysicsComponent::SetWorldTranslation(const glm::vec3 &translation) const
   {
      mDescriptor->SetTranslation(Converter::glmToBullet(translation));
   }

   void GhostPhysicsComponent::SetWorldRotator(const glm::quat &rotator) const
   {
      mDescriptor->SetRotator(Converter::glmToBullet(rotator));
   }

   ComponentType GhostPhysicsComponent::GetComponentType() const
   {
      return PHYSICS_COMPONENT;
   }
}