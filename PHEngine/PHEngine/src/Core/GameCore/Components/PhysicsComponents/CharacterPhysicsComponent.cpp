#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/GameCore/Actor.h"

#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"

#include "CharacterPhysicsComponent.h"

using namespace EngineMath;

namespace EnginePhysics
{
   CharacterPhysicsComponent::CharacterPhysicsComponent(PhysicsDescriptor* descriptor)
      : PhysicsComponent(descriptor)
      , characterController(static_cast<DynamicCharacterController*>(descriptor))
   {
      assert(characterController);
   }

   CharacterPhysicsComponent::~CharacterPhysicsComponent()
   {
   }

   void CharacterPhysicsComponent::Tick(const float deltaTime)
   {
      characterController->UpdateMotionWorldTransformLocalState(bIsTransformationDirty, deltaTime);

      if (bIsTransformationDirty)
      {
         const Actor* owner = GetOwner();

         owner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(characterController->GetTranslation()));

         Event::PhysicsSimulationUpdatedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::POST_EXECUTION, owner->GetName());
      }
   }

   void CharacterPhysicsComponent::SetWalkVelocity(const  glm::vec3& velocity)
   {
      const float m_acceleration_walk = 2;
      const float timeMult = 1;

      characterController->Walk(velocity * m_acceleration_walk * timeMult);
   }

   void CharacterPhysicsComponent::SetRunVelocity(const glm::vec3& velocity)
   {
      const float m_acceleration_run = 5;
      const float timeMult = 1;

      characterController->Walk(velocity * m_acceleration_run * timeMult);
   }

   void CharacterPhysicsComponent::SetJumpVelocity()
   {
      characterController->Jump();
   }
}