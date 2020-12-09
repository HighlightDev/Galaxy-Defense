#pragma once

#include <glm/vec3.hpp>

#include "PhysicsShapeBase.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

namespace EnginePhysics
{

   struct PhyBoxShape
      : public PhysicsShapeBase
   {
      PhyBoxShape(const glm::vec3& halfExtent)
         : PhysicsShapeBase(new btBoxShape(btVector3(halfExtent.x, halfExtent.y, halfExtent.z)))
      {
      }

      glm::vec3 GetHalfExtent() const {
         return Converter::bulletToGlm(static_cast<btBoxShape*>(mCollisionShape)->getHalfExtentsWithoutMargin());
      }
   };

}