#pragma once

#include "BoundingBox.h"

#include <vector>

namespace Game
{

   class BoundingBoxBuilder
   {
   public:

      BoundingBoxBuilder();

      ~BoundingBoxBuilder();

      static BoundingBox Build(const std::vector<float> meshPositions);
   };

}

