#pragma once

#include "PhysicsShapeBase.h"

namespace EnginePhysics
{

   struct PhySphereShape
      : public PhysicsShapeBase
   {

      PhySphereShape(const double radius)
         : PhysicsShapeBase(new btSphereShape(radius))
      {
      }

      double GetRadius() const {
         return static_cast<btSphereShape*>(mCollisionShape)->getRadius();
      }
   };

}
