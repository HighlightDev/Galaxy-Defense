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
   CharacterPhysicsComponent::CharacterPhysicsComponent(const std::string& gameObjectName, PhysicsDescriptor* descriptor)
      : PhysicsComponent(gameObjectName, descriptor)
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
         if (const auto& spOwner = GetOwner().lock())
         {
            spOwner->GetRootComponent()->SetTranslation(Converter::bulletToGlm(characterController->GetTranslation()));
            Event::PhysicsSimulationUpdatedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::POST_EXECUTION, spOwner->GetName());
         }
      }
   }

   void CharacterPhysicsComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);
      std::shared_ptr<SerializeDataCharacterPhysicsComponent> charPhysCompData = std::make_shared<SerializeDataCharacterPhysicsComponent>();
      charPhysCompData->ComponentName = GameObjectName;
      charPhysCompData->CapsuleHeight = characterController->GetCapsuleHeight();
      charPhysCompData->CapsuleRadius = characterController->GetCapsuleRadius();
      charPhysCompData->Mass = characterController->GetMass();
      charPhysCompData->StepHeight = characterController->GetStepHeight();

      actorData.ComponentsData.emplace_back(charPhysCompData);
   }

   void CharacterPhysicsComponent::SetWalkVelocity(const glm::vec3& velocity)
   {
      characterController->Walk(velocity);
   }

   void CharacterPhysicsComponent::SetJumpVelocity()
   {
      characterController->Jump();
   }
}