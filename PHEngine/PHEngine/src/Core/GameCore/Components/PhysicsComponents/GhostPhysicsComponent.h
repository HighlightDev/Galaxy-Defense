#pragma once

#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"

#include <glm/ext/quaternion_float.hpp>

using namespace EngineCore;

namespace EnginePhysics
{

   class GhostPhysicsComponent
       : public PhysicsComponent
   {
   protected:
      std::shared_ptr<EngineGOProperty<glm::vec3>> m_HitBoxScale;

   public:
      GhostPhysicsComponent(const PhysicsComponentData &data);

      virtual ~GhostPhysicsComponent();

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

   private:
      void SyncHitBoxScale(const glm::vec3 &scale);
   };
}
