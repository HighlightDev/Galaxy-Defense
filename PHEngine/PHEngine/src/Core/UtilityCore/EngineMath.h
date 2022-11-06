#pragma once

#include <algorithm>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <optional>
#include <vector>

namespace EngineMath
{
#define AXIS_RIGHT glm::vec3(1, 0, 0)
#define AXIS_UP glm::vec3(0, 1, 0)
#define AXIS_FORWARD glm::vec3(0, 0, 1)

   constexpr auto G = 9.8f;

   constexpr auto PI = 3.14159f;
   /************************************************************************/
   /*                       Convert from degrees to radians                */
   /************************************************************************/

#define DEG_TO_RAD(X) (glm::radians<float>(X))

   /************************************************************************/
   /*                       Convert from radians to degrees                */
   /************************************************************************/
#define RAD_TO_DEG(X) (glm::degrees<float>(X))

#define ENGINE_FLOAT_EPSILON 0.01f

   bool CompareFloats(const float X, const float Y);

   float LerpNormalizedFloat(const float src, const float dst, const float factor);
   float LerpFloat(const float x, const float x1, const float x2, const float y1, const float y2);

   glm::vec3 LerpVec3(const float t, const float t1, const float t2, const glm::vec3 &position1, const glm::vec3 &position2);
   glm::vec4 LerpVec4(const float t, const float t1, const float t2, const glm::vec4 &position1, const glm::vec4 &position2);

   glm::quat SLerpQuat(float t, const glm::quat &src, const glm::quat &dst);

   float GetDistancePlaneToPointVec3(const glm::vec3 &point, const glm::vec4 &plane, float);

   void TestAABBPlane(const glm::vec3 &origin, const glm::vec3 &extent, const glm::vec4 &plane, float &outDistanceOriginToPlane, float &outAbsExtentOnNormalProjected);

   bool TestPointInAABB(const glm::vec3 &aabbMin, const glm::vec3 &aabbMax, const glm::vec3 &point);

   glm::quat EulerAnglesToQuat(const glm::vec3 &eulerAngles);

   glm::mat4 BuildMirrorMatrix(const glm::vec4 &mirrorPlane);

   glm::vec3 QuatToEulerAngles(const glm::quat &rotationQuat);

   std::optional<glm::vec3> TestPlaneToPlaneToPlane(const glm::vec4 &plane1, const glm::vec4 &plane2, const glm::vec4 &plane3);

   float GaussFunction(const float x, const float sigma);

   std::vector<float> CalculateGaussNormalizedWeights(const uint32_t blurWidth);

}