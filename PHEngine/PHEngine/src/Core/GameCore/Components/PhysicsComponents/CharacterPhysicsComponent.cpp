#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Event/PhysicsDescriptorRemovedEvent.h"
#include "Core/GameCore/Actor.h"

#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"

#include "CharacterPhysicsComponent.h"

using namespace EngineMath;

namespace EnginePhysics
{
   CharacterPhysicsComponent::CharacterPhysicsComponent(const std::shared_ptr<PhysicsComponentData>& data)
      : PhysicsComponent(data)
      , characterController(static_cast<DynamicCharacterController*>(data->mPhysicsDescriptor))
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
            Event::PhysicsComponentUpdatedEvent::GetInstance()->SendEvent(Event::eExecutionOrder::POST_EXECUTION, spOwner->GetName());
         }
      }
   }

   void CharacterPhysicsComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);
      std::shared_ptr<SerializeDataCharacterPhysicsComponent> charPhysCompData = std::make_shared<SerializeDataCharacterPhysicsComponent>();
      charPhysCompData->ComponentName = EngineObjectName;
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