#pragma once

#include <glm/vec3.hpp>

namespace Game
{
   class BoundingBox
   {
      glm::vec3 mOrigin;
      glm::vec3 mExtent;
      glm::vec3 mScale;
      glm::vec3 mTranslation;

   public:

      BoundingBox(const glm::vec3& origin, const glm::vec3& extent);

      ~BoundingBox();

      void UpdateScale(const glm::vec3& scale);
      void UpdateTranslation(const glm::vec3& translation);
   };
}
