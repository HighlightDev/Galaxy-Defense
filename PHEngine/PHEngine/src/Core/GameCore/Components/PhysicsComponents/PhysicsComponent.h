#pragma once

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

#include <glm/ext/quaternion_float.hpp>

using namespace EngineCore;

namespace EngineCore
{
   class Actor;
};

namespace EnginePhysics
{

   class PhysicsComponent
       : public Component
   {
   protected:
      std::shared_ptr<PhysicsDescriptor> mDescriptor;

      bool bIsTransformationDirty;

   public:
      PhysicsComponent(const std::shared_ptr<PhysicsComponentData> &data);

      ~PhysicsComponent() override;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) = 0;

      void OnPostOwnerInitialized() override;

      void SetIsEnabled(const bool isEnabled) override;

      void SetOwner(const std::weak_ptr<Actor> &ownerActor) override;

      std::shared_ptr<PhysicsDescriptor> GetDescriptor() const;

      bool IsTransformDirty() const;

      eComponentType GetComponentType() const override;

      glm::vec3 GetWorldTranslation() const;

      glm::quat GetWorldRotator() const;

      void SetWorldTranslation(const glm::vec3 &translation) const;

      void SetWorldRotator(const glm::quat &rotator) const;

      void CleanUp() override;
   };
}
