#pragma once

#include "Core/GameCore/BoundingBox.h"

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <array>

using namespace Game;

namespace Graphics
{

   class CameraFrustum
   {
      std::array<glm::vec4, 6> mPlanes;
      glm::mat4 viewProjectionMatrix;
   public:

      CameraFrustum();

      ~CameraFrustum();

      void ConstructFromViewProjectionMatrix(const glm::mat4& viewMatrix, const glm::mat4& projectioMatrix);

      bool IsIntersectionWithPointVec3(const glm::vec3& point) const;

      bool IsIntersectionWithSphere(const glm::vec3& origin, const float radius) const;

      bool IsIntersectionWithBoundingBox(const BoundingBox& boundingBox, const bool doSphereTest = false) const;
   };

}
