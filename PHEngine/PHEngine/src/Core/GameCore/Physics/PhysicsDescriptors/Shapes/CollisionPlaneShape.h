#pragma once

#include <glm/vec3.hpp>

#include "CollisionShapeBase.h"

namespace EnginePhysics
{

   struct CollisionPlaneShape
      : public CollisionShapeBase
   {
      CollisionPlaneShape(const glm::vec3& normal, const double d)
         : CollisionShapeBase(new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), d))
      {
      }
   };

}
