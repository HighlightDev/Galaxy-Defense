#pragma once

#include "Core/GameCore/BoundingBox3D.h"

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <array>

using namespace EngineCore;

namespace Graphics
{
   enum class eFrustumPlaneName
   {
      Left,
      Right,
      Bottom,
      Top,
      Near,
      Far
   };

   class CameraFrustum
   {
      std::array<glm::vec4, 6> mPlanes;

   public:

      CameraFrustum();

      ~CameraFrustum();

      void ConstructFromViewProjectionMatrix(const glm::mat4& viewMatrix, const glm::mat4& projectioMatrix);

      bool IsIntersectionWithPointVec3(const glm::vec3& point) const;

      bool IsIntersectionWithBox(const glm::vec3& origin, const glm::vec3& extent) const;

      bool CollidesWithBoundingBox(const BoundingBox3D& boundingBox) const;

      static CameraFrustum GetConstructedFromViewProjectionMatrices(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

      glm::vec4 GetPlaneByName(const eFrustumPlaneName planeName) const;
   };

}
