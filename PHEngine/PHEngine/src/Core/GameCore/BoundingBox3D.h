#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <array>

namespace EngineCore
{
   class BoundingBox3D
   {
      glm::vec3 mOrigin;

      glm::vec3 mHalfExtent;

   public:

      BoundingBox3D();

      BoundingBox3D(const glm::vec3& origin, const glm::vec3& halfExtent);

      ~BoundingBox3D();

      glm::vec3 GetMax() const;

      glm::vec3 GetMin() const;

      glm::vec3 GetOrigin() const;

      glm::vec3 GetHalfExtent() const;

      bool IsIntersectionWithBox(const BoundingBox3D& boundingBox) const;

      std::array<glm::vec3, 8> GetBoundPositions() const;
   };
}
