#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsDescriptors/PhysicsDescriptor.h"

namespace Game {

   class PhysicsComponent 
      : public Component
   {
      PhysicsDescriptor* mDescriptor;

   public:

      PhysicsComponent(PhysicsDescriptor* descriptor);

      virtual ~PhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      void PostPhysicsInit();
   };
}

