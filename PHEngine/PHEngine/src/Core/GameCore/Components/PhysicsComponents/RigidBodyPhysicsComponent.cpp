#include "RigidBodyPhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

using namespace EngineMath;

namespace EnginePhysics
{
   RigidBodyPhysicsComponent::RigidBodyPhysicsComponent(const std::shared_ptr<PhysicsComponentData>& data)
      : PhysicsComponent(data)
   {
   }

   RigidBodyPhysicsComponent::~RigidBodyPhysicsComponent()
   {
   }

   void RigidBodyPhysicsComponent::Tick(const float deltaTime)
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

               Event::PhysicsComponentUpdatedEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, spOwner->GetName());
            }
         }
      }
   }

   void RigidBodyPhysicsComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      SerializeDataActor& actorData = Component::GetSerializeDataActor(dataContainer);
      auto physCompData = SerializeHelper::GetSerializeDataPhysicsComponent(this);
      physCompData->ComponentName = EngineObjectName;

      actorData.ComponentsData.emplace_back(physCompData);
   }
}