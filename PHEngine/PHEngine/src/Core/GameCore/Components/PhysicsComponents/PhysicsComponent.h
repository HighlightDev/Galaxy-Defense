#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

using namespace EnginePhysics;

namespace Game {

   class PhysicsComponent 
      : public Component
   {
      PhysicsDescriptor* mDescriptor;
      
      bool bIsTransformationDirty;

   public:

      PhysicsComponent(PhysicsDescriptor* descriptor);

      virtual ~PhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      void PostPhysicsInit();

      PhysicsDescriptor* GetDescriptor();

      bool IsTransformDirty() const;

      virtual uint64_t GetComponentType() const override;

   };
}

