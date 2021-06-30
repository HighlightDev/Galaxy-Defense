#pragma once

#include <stdint.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Graphics
{
   struct ViewPortInfo
   {
      int32_t OriginX;
      int32_t OriginY;
      int32_t Width;
      int32_t Height;

      ViewPortInfo(int32_t originX, int32_t originY, int32_t width, int32_t height)
         : OriginX(originY)
         , OriginY(originY)
         , Width(width)
         , Height(height)
      {
      }

      ViewPortInfo()
         : OriginX(0)
         , OriginY(0)
         , Width(0)
         , Height(0)
      {
      }

      ViewPortInfo(int32_t originX, int32_t originY, const glm::ivec2& resolution)
         : OriginX(originY)
         , OriginY(originY)
         , Width(resolution.x)
         , Height(resolution.y)
      {
      }

      ViewPortInfo(const glm::ivec4& viewport)
         : OriginX(viewport.x)
         , OriginY(viewport.y)
         , Width(viewport.z)
         , Height(viewport.w)
      {
      }

      explicit operator glm::ivec4 () const {
         return glm::ivec4(OriginX, OriginY, Width, Height);
      }
   };
}