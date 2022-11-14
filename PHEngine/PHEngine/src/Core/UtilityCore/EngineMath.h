#pragma once

#include <algorithm>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <optional>
#include <vector>

namespace EngineMath
{
   /************************************************************************/
   /*                             CONSTANTS                                */
   /************************************************************************/
   extern float G;
   extern float PI;
   extern float ENGINE_FLOAT_EPSILON;
   extern glm::vec3 AXIS_RIGHT;
   extern glm::vec3 AXIS_UP;
   extern glm::vec3 AXIS_FORWARD;

   /************************************************************************/
   /*                       Convert from degrees to radians                */
   /************************************************************************/
#define DEG_TO_RAD(X) (glm::radians<float>(X))

   /************************************************************************/
   /*                       Convert from radians to degrees                */
   /************************************************************************/
#define RAD_TO_DEG(X) (glm::degrees<float>(X))

   /************************************************************************/
   /*                           Math Helper Functions                      */
   /************************************************************************/
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

   bool CheckSimilarityVec2(const glm::vec2 &left, const glm::vec2 &right);
   bool CheckSimilarityIVec2(const glm::ivec2 &left, const glm::ivec2 &right);

}