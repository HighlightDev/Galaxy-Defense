#pragma once

#include "PhyShapeBase.h"

namespace Game
{

   struct PhySphereShape
      : public PhyShapeBase
   {

      PhySphereShape(const float radius)
         : PhyShapeBase(new btSphereShape(btScalar(radius)))
      {
      }
   };

}
