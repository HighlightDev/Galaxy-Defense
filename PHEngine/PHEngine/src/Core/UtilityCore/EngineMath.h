#pragma once

#include <algorithm>
#include <glm/trigonometric.hpp>

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


   bool CompareFloats(const float X, const float Y);
}