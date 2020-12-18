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

   public:

      CameraFrustum();

      ~CameraFrustum();

      void ConstructFromViewProjectionMatrix(const glm::mat4& viewMatrix, const glm::mat4& projectioMatrix);

      bool IsIntersectionWithPointVec3(const glm::vec3& point) const;

      bool IsIntersectionWithBox(const glm::vec3& origin, const glm::vec3& extent) const;

      bool CollidesWithBoundingBox(const BoundingBox& boundingBox) const;
   };

}
