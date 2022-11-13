#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <array>

namespace EngineCore
{
   namespace GUI
   {
      class BoundingBox2D
      {
         glm::vec2 mOrigin;

         glm::vec2 mHalfExtent;

      public:
         BoundingBox2D();

         BoundingBox2D(const glm::vec2 &origin, const glm::vec2 &halfExtent);

         ~BoundingBox2D();

         glm::vec2 GetMax() const;

         glm::vec2 GetMin() const;

         glm::vec2 GetOrigin() const;

         glm::vec2 GetHalfExtent() const;
      };
   }
}
