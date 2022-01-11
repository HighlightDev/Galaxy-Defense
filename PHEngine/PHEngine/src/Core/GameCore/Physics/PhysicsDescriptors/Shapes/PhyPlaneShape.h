#pragma once

#include <glm/vec3.hpp>

#include "PhysicsShapeBase.h"

namespace EnginePhysics
{

   struct PhyPlaneShape
      : public PhysicsShapeBase
   {
      PhyPlaneShape(const glm::vec3& normal, const double d)
         : PhysicsShapeBase(new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), d))
      {
      }
   };

}
