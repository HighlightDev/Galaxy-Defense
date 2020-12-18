#include "EngineMath.h"

#include <glm/geometric.hpp>

namespace EngineMath
{
   bool CompareFloats(const float X, const float Y)
   {
      const float absX = std::abs(X);
      const float absY = std::abs(Y);
      const bool bResult = std::abs(absX - absY) <= ENGINE_FLOAT_EPSILON;
      return bResult;
   }

   float LerpFloat(const float src, const float dst, const float factor)
   {
      assert(factor <= 1 && factor >= 0);
      return (dst - src) * factor + src;
   }

   glm::vec3 LerpVec3(float t, float t1, float t2, const glm::vec3& position1, const glm::vec3& position2)
   {
      glm::vec3 resultPosition = glm::vec3(0);

      float x_delta = t2 - t1;
      float x_zero_offset = t - t1;

      resultPosition.x = ((position2.x - position1.x) / x_delta) * x_zero_offset + position1.x;
      resultPosition.y = ((position2.y - position1.y) / x_delta) * x_zero_offset + position1.y;
      resultPosition.z = ((position2.z - position1.z) / x_delta) * x_zero_offset + position1.z;

      return resultPosition;
   }

   float GetDistancePlaneToPointVec3(const glm::vec3& point, const glm::vec4& plane, float w)
   {
      const glm::vec3& normal = glm::vec3(plane);
      const float distance = glm::dot(point, normal) + w;
      return distance;
   }

   void TestAABBPlane(const glm::vec3& origin, const glm::vec3& extent, const glm::vec4& plane, float& outDistanceOriginToPlane, float& outAbsExtentOnNormalProjected) 
   {
      const glm::vec3& planeNormal = glm::vec3(plane);
      const glm::vec3& absNormal = glm::abs(planeNormal);
      outAbsExtentOnNormalProjected = glm::abs(glm::dot(extent, absNormal));
      outDistanceOriginToPlane = glm::dot(planeNormal, origin) + plane.w;
   }
}