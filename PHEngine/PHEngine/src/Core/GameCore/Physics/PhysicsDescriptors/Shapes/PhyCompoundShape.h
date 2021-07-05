#pragma once

#include <vector>
#include <map>

#include "PhysicsShapeBase.h"
#include "Core/GameCore/Components/Transform.h"

using namespace Game;

namespace EnginePhysics
{

   struct PhyCompoundShape
      : public PhysicsShapeBase
   {
   private:

      std::vector<std::pair<PhysicsShapeBase*, NoScaleEulerRotationTransform>> mChildShapes;

   public:

      PhyCompoundShape();

      virtual ~PhyCompoundShape();

      void AddChildShape(const NoScaleEulerRotationTransform& childTransform, PhysicsShapeBase* childShape);

      const std::vector<std::pair<PhysicsShapeBase*, NoScaleEulerRotationTransform>>& GetChildShapes() const;
   };

}