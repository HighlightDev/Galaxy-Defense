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

      for (size_t i = 3; i < countVertices; i += 3)
      {
         const float vertex_x = meshPositions[i + 0];
         const float vertex_y = meshPositions[i + 1];
         const float vertex_z = meshPositions[i + 2];

         max.x = std::max(max.x, vertex_x);
         max.y = std::max(max.y, vertex_y);
         max.z = std::max(max.z, vertex_z);

         min.x = std::min(min.x, vertex_x);
         min.y = std::min(min.y, vertex_y);
         min.z = std::min(min.z, vertex_z);
      }

      glm::vec3 halfExtent = (max - min) / 2.0f;
      glm::vec3 origin = min + halfExtent;

      return BoundingBox(origin, halfExtent);
   }
}
