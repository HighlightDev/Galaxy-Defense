#pragma once

#include "PhysicsShapeBase.h"

namespace EnginePhysics
{

   struct PhyCapsuleShape
      : public PhysicsShapeBase
   {
      PhyCapsuleShape(const float radius, const float height)
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