#pragma once

#include "PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

namespace EnginePhysics {

   class CharacterPhysicsComponent
      : public PhysicsComponent
   {
      std::shared_ptr<DynamicCharacterController> characterController;

   public:

      CharacterPhysicsComponent(const std::shared_ptr<PhysicsComponentData>& data);

      ~CharacterPhysicsComponent() override;

      void Tick(const float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      void SetWalkVelocity(const glm::vec3& velocity);

      void SetJumpVelocity();

   };
}

