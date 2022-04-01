#include "EngineMath.h"

#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>

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

   glm::vec3 LerpVec3(float t, float t1, float t2, const glm::vec3 &position1, const glm::vec3 &position2)
   {
      glm::vec3 resultPosition = glm::vec3(0);

      float x_delta = t2 - t1;
      float x_zero_offset = t - t1;

      resultPosition.x = ((position2.x - position1.x) / x_delta) * x_zero_offset + position1.x;
      resultPosition.y = ((position2.y - position1.y) / x_delta) * x_zero_offset + position1.y;
      resultPosition.z = ((position2.z - position1.z) / x_delta) * x_zero_offset + position1.z;

      return resultPosition;
   }

   glm::quat SLerpQuat(float t, const glm::quat &src, const glm::quat &dst)
   {
      return glm::lerp(src, dst, t);
   }

   float GetDistancePlaneToPointVec3(const glm::vec3 &point, const glm::vec4 &plane, float w)
   {
      const glm::vec3 &normal = glm::vec3(plane);
      const float distance = glm::dot(point, normal) + w;
      return distance;
   }

   void TestAABBPlane(const glm::vec3 &origin, const glm::vec3 &extent, const glm::vec4 &plane, float &outDistanceOriginToPlane, float &outAbsExtentOnNormalProjected)
   {
      const glm::vec3 &planeNormal = glm::vec3(plane);
      const glm::vec3 &absNormal = glm::abs(planeNormal);
      outAbsExtentOnNormalProjected = glm::abs(glm::dot(extent, absNormal));
      outDistanceOriginToPlane = glm::dot(planeNormal, origin) + plane.w;
   }

   bool TestPointInAABB(const glm::vec3 &aabbMin, const glm::vec3 &aabbMax, const glm::vec3 &point)
   {
      if ((point.x < aabbMin.x || point.y < aabbMin.y || point.z < aabbMin.z) ||
          (point.x > aabbMax.x || point.y > aabbMax.y || point.z > aabbMax.z))
      {
         return false;
      }
      return true;
   }

   glm::quat EulerAnglesToQuat(const glm::vec3 &eulerAngles)
   {
      return glm::quat(glm::vec3(DEG_TO_RAD(eulerAngles.x), DEG_TO_RAD(eulerAngles.y), DEG_TO_RAD(eulerAngles.z)));
   }

   glm::mat4 BuildMirrorMatrix(const glm::vec4 &mirrorPlane)
   {
      const glm::mat4 mirrorMatrix(
          glm::vec4(-2.f * mirrorPlane.x * mirrorPlane.x + 1.f, -2.f * mirrorPlane.y * mirrorPlane.x, -2.f * mirrorPlane.z * mirrorPlane.x, 0.f),
          glm::vec4(-2.f * mirrorPlane.x * mirrorPlane.y, -2.f * mirrorPlane.y * mirrorPlane.y + 1.f, -2.f * mirrorPlane.z * mirrorPlane.y, 0.f),
          glm::vec4(-2.f * mirrorPlane.x * mirrorPlane.z, -2.f * mirrorPlane.y * mirrorPlane.z, -2.f * mirrorPlane.z * mirrorPlane.z + 1.f, 0.f),
          glm::vec4(2.f * mirrorPlane.x * mirrorPlane.w, 2.f * mirrorPlane.y * mirrorPlane.w, 2.f * mirrorPlane.z * mirrorPlane.w, 1.f));

      return mirrorMatrix;
   }

   glm::vec3 QuatToEulerAngles(const glm::quat &rotationQuat)
   {
      static constexpr float radToDeg = 180.f / 3.14159f;
      return glm::eulerAngles(rotationQuat) * radToDeg;
   }
}