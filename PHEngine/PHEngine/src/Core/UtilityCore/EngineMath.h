#pragma once

#include <algorithm>

namespace EngineUtility
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

   struct CMP
   {
      static bool Process(const float X, const float Y)
      {
         const float absX = std::abs(X);
         const float absY = std::abs(Y);
         const bool bResult = std::abs(absX - absY) <= FLT_EPSILON;
         return bResult;
      }
   };
}