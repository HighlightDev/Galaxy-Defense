#include "BoundingBoxBuilder.h"

#include <algorithm>

namespace EngineCore
{

   BoundingBoxBuilder::BoundingBoxBuilder()
   {
   }

   BoundingBoxBuilder::~BoundingBoxBuilder()
   {
   }

   BoundingBox BoundingBoxBuilder::Build(const std::vector<float> meshPositions)
   {
      const size_t positionsNum = meshPositions.size();

      glm::vec3 max = glm::vec3(meshPositions[0], meshPositions[1], meshPositions[2]);
      glm::vec3 min = max;

      for (size_t i = 3; i < positionsNum; i += 3)
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

   BoundingBox BoundingBoxBuilder::GetTransformedBoundingBox(const BoundingBox& localSpaceBb, const glm::mat4& transformMatrix)
   {
      std::array<glm::vec3, 8> bbPoints = localSpaceBb.GetBoundPositions();

      glm::vec4 startPoint = transformMatrix * glm::vec4(*bbPoints.begin(), 1.0f);
      glm::vec3 maxPoint = startPoint;
      glm::vec3 minPoint = maxPoint;

      for (auto pointIt = std::next(bbPoints.begin(), 1); pointIt != bbPoints.end(); ++pointIt)
      {
         const glm::vec4& result = transformMatrix * glm::vec4(*pointIt, 1.0f);

         maxPoint.x = glm::max(result.x, maxPoint.x);
         maxPoint.y = glm::max(result.y, maxPoint.y);
         maxPoint.z = glm::max(result.z, maxPoint.z);

         minPoint.x = glm::min(result.x, minPoint.x);
         minPoint.y = glm::min(result.y, minPoint.y);
         minPoint.z = glm::min(result.z, minPoint.z);
      }

      const glm::vec3& halfExtent = glm::abs(maxPoint - minPoint) / 2.0f;
      const glm::vec3& origin = minPoint + halfExtent;

      return BoundingBox(origin, halfExtent);
   }
}
