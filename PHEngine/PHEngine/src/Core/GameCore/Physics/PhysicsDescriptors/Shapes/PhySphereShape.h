#pragma once

#include "PhyShapeBase.h"

namespace EnginePhysics
{

   struct PhySphereShape
      : public PhyShapeBase
   {

      PhySphereShape(const float radius)
         : PhyShapeBase(new btSphereShape(btScalar(radius)))
      {
      }

      float GetRadius() const {
         return static_cast<btCapsuleShape*>(mCollisionShape)->getRadius();
      }
   };

}
