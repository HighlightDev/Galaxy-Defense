#pragma once

#include <vector>
#include <map>
#include <memory>

#include "CollisionShapeBase.h"
#include "Core/GameCore/Components/Transform.h"

using namespace EngineCore;

namespace EnginePhysics
{

   struct CollisionCompoundShape
       : public CollisionShapeBase
   {
   private:
      std::vector<std::pair<std::shared_ptr<CollisionShapeBase>, NoScaleEulerRotationTransform>> mChildShapes;

   public:
      CollisionCompoundShape();

      void AddChildShape(const NoScaleEulerRotationTransform &childTransform, const std::shared_ptr<CollisionShapeBase> &childShape);

      const std::vector<std::pair<std::shared_ptr<CollisionShapeBase>, NoScaleEulerRotationTransform>> &GetChildShapes() const;
   };

}