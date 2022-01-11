#pragma once

#include "PhysicsShapeBase.h"

namespace EnginePhysics
{

   struct PhyCapsuleShape
      : public PhysicsShapeBase
   {
      PhyCapsuleShape(const double radius, const double height)
         : PhysicsShapeBase(new btCapsuleShape(radius, height))
      {
      }

      float GetRadius() const {
         return static_cast<btCapsuleShape*>(mCollisionShape)->getRadius();
      }

      float GetHeight() const {
         return static_cast<btCapsuleShape*>(mCollisionShape)->getHalfHeight() * 2.0f;
      }
   };

}