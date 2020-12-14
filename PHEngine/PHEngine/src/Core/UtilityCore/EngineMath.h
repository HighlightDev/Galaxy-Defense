#pragma once

#include <algorithm>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EngineMath
{
#define AXIS_RIGHT     glm::vec3(1, 0, 0)
#define AXIS_UP	       glm::vec3(0, 1, 0)
#define AXIS_FORWARD   glm::vec3(0, 0, 1)

   constexpr auto PI = 3.14f;
   /************************************************************************/
   /*                       Convert from degrees to radians                */
   /************************************************************************/

#define DEG_TO_RAD(X) (glm::radians<float>(X ))

   /************************************************************************/
   /*                       Convert from radians to degrees                */
   /************************************************************************/
#define RAD_TO_DEG(X) (glm::degrees<float>(X))


#define ENGINE_FLOAT_EPSILON 0.01f

   bool CompareFloats(const float X, const float Y);
   
   float LerpFloat(const float src, const float dst, const float factor);

   glm::vec3 LerpVec3(float t, float t1, float t2, const glm::vec3& position1, const glm::vec3& position2);

   float GetDistancePlaneToPointVec3(const glm::vec3& point, const glm::vec4& plane);

}