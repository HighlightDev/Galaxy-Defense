#pragma once

#include <glm/vec3.hpp>

#include "PhyShapeBase.h"

namespace EnginePhysics
{

   struct PhyBoxShape
      : public PhyShapeBase
   {
      PhyBoxShape(const glm::vec3& halfExtent)
         : PhyShapeBase(new btBoxShape(btVector3(halfExtent.x, halfExtent.y, halfExtent.z)))
      {
      }
   };

}