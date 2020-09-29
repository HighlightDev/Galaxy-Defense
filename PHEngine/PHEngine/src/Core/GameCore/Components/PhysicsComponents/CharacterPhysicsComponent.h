#pragma once

#include "PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"

namespace EnginePhysics {

   class CharacterPhysicsComponent
      : public PhysicsComponent
   {
      DynamicCharacterController* characterController;

   public:

      CharacterPhysicsComponent(const std::string& gameObjectName, PhysicsDescriptor* descriptor);

      virtual ~CharacterPhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      void SetWalkVelocity(const glm::vec3& velocity);

      void SetJumpVelocity();

   };
}

