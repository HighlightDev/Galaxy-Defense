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
         glm::ivec2 mOrigin;

         glm::ivec2 mHalfExtent;

      public:
         BoundingBox2D();

         BoundingBox2D(const glm::ivec2 &origin, const glm::ivec2 &halfExtent);

         ~BoundingBox2D();

         glm::ivec2 GetMax() const;

         glm::ivec2 GetMin() const;

         glm::ivec2 GetOrigin() const;

         glm::ivec2 GetHalfExtent() const;
      };
   }
}
