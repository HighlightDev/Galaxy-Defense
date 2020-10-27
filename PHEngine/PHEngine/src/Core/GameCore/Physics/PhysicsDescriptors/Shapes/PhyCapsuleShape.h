#pragma once

#include "PhyShapeBase.h"

namespace EnginePhysics
{

   struct PhyCapsuleShape
      : public PhyShapeBase
   {
      PhyCapsuleShape(const float radius, const float height)
         : PhyShapeBase(new btCapsuleShape(radius, height))
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