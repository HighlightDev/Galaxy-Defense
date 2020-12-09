#pragma once

#include <glm/vec3.hpp>

namespace Game
{
   class BoundingBox
   {
      glm::vec3 mOrigin;

      glm::vec3 mExtent;

   public:

      BoundingBox();

      BoundingBox(const glm::vec3& origin, const glm::vec3& extent);

      ~BoundingBox();

      glm::vec3 GetTransformedOrigin(const glm::vec3& translation) const;

      glm::vec3 GetTransformedExtent(const glm::vec3& scale) const;

      glm::vec3 GetOrigin() const;

      glm::vec3 GetExtent() const;
   };
}
