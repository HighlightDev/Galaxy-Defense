#pragma once

#include <stdint.h>

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
   };
}