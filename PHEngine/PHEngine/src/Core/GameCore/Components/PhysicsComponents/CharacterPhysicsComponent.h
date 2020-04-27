#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"

namespace EnginePhysics {

   class CharacterPhysicsComponent
      : public Component
   {
      DynamicCharacterController* characterController;

      bool bIsTransformationDirty;

   public:

      CharacterPhysicsComponent(btDiscreteDynamicsWorld* dynamicWorld, const glm::vec3& spawnPos);

      virtual ~CharacterPhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      void PostPhysicsInit();

      bool IsTransformDirty() const;

      virtual uint64_t GetComponentType() const override;

      void SetWalkVelocity(const glm::vec3& velocity);

      void SetRunVelocity(const glm::vec3& velocity);

      void SetJumpVelocity();

   private :

      void TickCharacterControllerPhysics();

   };
}

