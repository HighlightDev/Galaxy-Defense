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
   };

}