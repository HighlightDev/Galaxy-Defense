#pragma once

#include "PhysicsShapeBase.h"

namespace EnginePhysics
{

   struct PhySphereShape
      : public PhysicsShapeBase
   {

      PhySphereShape(const float radius)
         : PhysicsShapeBase(new btSphereShape(btScalar(radius)))
      {
      }

      float GetRadius() const {
         return static_cast<btSphereShape*>(mCollisionShape)->getRadius();
      }
   };

}
