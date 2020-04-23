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
      : characterController(new DynamicCharacterController(dynamicWorld, Converter::glmToBullet(spawnPos), 1, 2.5, 10, 1.0f))
      , bIsTransformationDirty(true)
   {
   }

   CharacterPhysicsComponent::~CharacterPhysicsComponent()
   {
   }

   void CharacterPhysicsComponent::Tick(const float deltaTime)
   {
      bool bIsDirty = true;

      bIsTransformationDirty = bIsDirty;

      TickCharacterControllerPhysics();

      if (bIsDirty)
      {
         const Actor* owner = GetOwner();

         owner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(characterController->GetPosition()));

         Event::PhysicsSimulationUpdatedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::POST_EXECUTION, owner->GetName());
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

   void CharacterPhysicsComponent::SetWalkVelocity(const  glm::vec3& velocity)
   {
      const float m_acceleration_walk = 5;
      const float timeMult = 1;

      characterController->Walk(velocity * m_acceleration_walk * timeMult);
   }

   void CharacterPhysicsComponent::SetRunVelocity(const glm::vec3& velocity)
   {
      const float m_acceleration_run = 15;
      const float timeMult = 1;

      characterController->Walk(velocity * m_acceleration_run * timeMult);
   }

   void CharacterPhysicsComponent::SetJumpVelocity()
   {
      characterController->Jump();
   }

   void CharacterPhysicsComponent::TickCharacterControllerPhysics()
   {
      characterController->Update();
   }
}