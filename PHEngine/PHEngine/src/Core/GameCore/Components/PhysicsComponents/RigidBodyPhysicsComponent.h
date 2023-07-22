#pragma once

#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

#include <glm/ext/quaternion_float.hpp>

using namespace EngineCore;

namespace EnginePhysics {

   class RigidBodyPhysicsComponent 
      : public PhysicsComponent
   {

   public:

      RigidBodyPhysicsComponent(const std::shared_ptr<PhysicsComponentData>& data);

      ~RigidBodyPhysicsComponent() override;

      void Tick(const float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

   };
}

