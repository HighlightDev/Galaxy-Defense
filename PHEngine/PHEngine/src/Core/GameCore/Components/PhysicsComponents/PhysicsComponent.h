#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include <glm/ext/quaternion_float.hpp>

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

      virtual ComponentType GetComponentType() const override;

      glm::vec3 GetWorldTranslation() const;

      glm::quat GetWorldRotator() const;

      void SetWorldTranslation(const glm::vec3& translation) const;

      void SetWorldRotator(const glm::quat& rotator) const;
   };
}

