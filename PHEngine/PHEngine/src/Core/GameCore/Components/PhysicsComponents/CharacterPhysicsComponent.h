#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/CharacterController.h"

namespace Game {

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

   };
}

