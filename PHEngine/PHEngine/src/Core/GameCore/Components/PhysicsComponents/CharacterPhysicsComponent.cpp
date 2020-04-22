#include "PhysicsComponent.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"

#include "CharacterPhysicsComponent.h"

using namespace EngineMath;

namespace Game
{
   CharacterPhysicsComponent::CharacterPhysicsComponent(btDiscreteDynamicsWorld* dynamicWorld, const glm::vec3& spawnPos)
      : characterController(new DynamicCharacterController(dynamicWorld, Converter::glmToBullet(spawnPos), 1, 2, 10, 0.5f))
      , bIsTransformationDirty(true)
   {
   }

   CharacterPhysicsComponent::~CharacterPhysicsComponent()
   {
   }

   void CharacterPhysicsComponent::Tick(const float deltaTime)
   {
      bool bIsDirty = false;

      //mDescriptor->UpdateMotionWorldTransformLocalState(bIsDirty);

      bIsTransformationDirty = bIsDirty;

      if (bIsDirty)
      {
         const Actor* owner = GetOwner();

         //owner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(mDescriptor->GetTranslation()));
         //owner->GetBaseRootComponent()->SetRotator(Converter::bulletToGlm(mDescriptor->GetRotator()));

         Event::PhysicsSimulationUpdatedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::PRE_EXECUTION, owner->GetName());
      }
   }

   bool CharacterPhysicsComponent::IsTransformDirty() const
   {
      return bIsTransformationDirty;
   }

   void CharacterPhysicsComponent::PostPhysicsInit()
   {
   }

   uint64_t CharacterPhysicsComponent::GetComponentType() const
   {
      return PHYSICS_COMPONENT;
   }
}