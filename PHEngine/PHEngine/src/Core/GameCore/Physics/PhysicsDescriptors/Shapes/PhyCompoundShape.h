#pragma once

#include "PhysicsShapeBase.h"
#include "Core/GameCore/Components/Transform.h"

using namespace Game;

namespace EnginePhysics
{

   struct PhyCompoundShape
      : public PhysicsShapeBase
   {
      PhyCompoundShape();

      virtual ~PhyCompoundShape();

      void AddChildShape(const NoScaleEulerRotationTransform& childTransform, PhysicsShapeBase* childShape);
   };

}