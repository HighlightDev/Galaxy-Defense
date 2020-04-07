#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsDescriptors/PhysicsDescriptor.h"

namespace Game {

   class PhysicsComponent 
      : public Component
   {
      PhysicsDescriptor* mDescriptor;
      
      glm::mat4 mTransformMatrix;

      bool bIsDirty;

   public:

      PhysicsComponent(PhysicsDescriptor* descriptor);

      virtual ~PhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      void PostPhysicsInit();

      bool IsTransformDirty() const;

      glm::mat4 GetTransformMatrix() const;
      
      virtual uint64_t GetComponentType() const override;

   private:

      glm::mat4 GetWorldTransformMatrixFromArray(float* mat) const;
   };
}

