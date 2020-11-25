#include "BoundingBoxBuilder.h"

#include <algorithm>

namespace Game
{

   BoundingBoxBuilder::BoundingBoxBuilder()
   {
   }

   BoundingBoxBuilder::~BoundingBoxBuilder()
   {
   }

   BoundingBox BoundingBoxBuilder::Build(const std::vector<float> meshPositions)
   {
      const size_t countVertices = meshPositions.size() / 3;

      glm::vec3 max = glm::vec3(meshPositions[0], meshPositions[1], meshPositions[2]);
      glm::vec3 min = max;

      for (size_t i = 1; i < countVertices; ++i)
      {
         const float vertex_x = meshPositions[0];
         const float vertex_y = meshPositions[1];
         const float vertex_z = meshPositions[2];

         max.x = std::max(max.x, vertex_x);
         max.y = std::max(max.y, vertex_y);
         max.z = std::max(max.z, vertex_z);

         min.x = std::min(min.x, vertex_x);
         min.y = std::min(min.y, vertex_y);
         min.z = std::min(min.z, vertex_z);
      }

      return BoundingBox(glm::vec3(0), max - min);
   }
}
