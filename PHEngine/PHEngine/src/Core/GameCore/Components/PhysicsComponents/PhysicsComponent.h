#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

using namespace Game;

namespace EnginePhysics {

   class PhysicsComponent 
      : public Component
   {
   protected:

      PhysicsDescriptor* mDescriptor;
      
      bool bIsTransformationDirty;

   public:

      PhysicsComponent(const std::string& gameObjectName, PhysicsDescriptor* descriptor);

      virtual ~PhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      void PostPhysicsInit();

      PhysicsDescriptor* GetDescriptor();

      bool IsTransformDirty() const;

      virtual uint64_t GetComponentType() const override;

   };
}

