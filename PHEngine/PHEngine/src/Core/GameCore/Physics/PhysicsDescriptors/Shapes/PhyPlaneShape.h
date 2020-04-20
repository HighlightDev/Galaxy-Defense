#pragma once

#include <glm/vec3.hpp>

#include "PhyShapeBase.h"

namespace Game
{

   struct PhyPlaneShape
      : public PhyShapeBase
   {
      PhyPlaneShape(const glm::vec3& normal, const float d)
         : PhyShapeBase(new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), d))
      {
      }
   };

}
