#pragma once

#include "BoundingBox3D.h"

#include <vector>

namespace EngineCore
{

   class BoundingBoxBuilder
   {
   public:

      BoundingBoxBuilder();

      ~BoundingBoxBuilder();

      BoundingBox3D Build(const std::vector<float>& meshPositions);

      static BoundingBox3D GetTransformedBoundingBox(const BoundingBox3D& localSpaceBb, const glm::mat4& transformMatrix);
   };

}

