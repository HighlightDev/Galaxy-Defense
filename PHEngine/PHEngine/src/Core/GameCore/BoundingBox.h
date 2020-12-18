#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <array>

namespace Game
{
   class BoundingBox
   {
      glm::vec3 mOrigin;

      glm::vec3 mHalfExtent;

   public:

      BoundingBox();

      BoundingBox(const glm::vec3& origin, const glm::vec3& halfExtent);

      ~BoundingBox();

      glm::vec3 GetOrigin() const;

      glm::vec3 GetHalfExtent() const;

      std::array<glm::vec3, 8> GetBoundPositions() const;
   };
}
