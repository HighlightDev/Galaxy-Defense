#pragma once

#include "PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

namespace EnginePhysics {

   class CharacterPhysicsComponent
      : public PhysicsComponent
   {
      DynamicCharacterController* characterController;

   public:

      CharacterPhysicsComponent(const PhysicsComponentData& data);

      virtual ~CharacterPhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      void SetWalkVelocity(const glm::vec3& velocity);

      void SetJumpVelocity();

   };
}

