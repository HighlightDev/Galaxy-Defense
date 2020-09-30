#include "EngineMath.h"

namespace EngineMath
{
   bool CompareFloats(const float X, const float Y)
   {
      const float absX = std::abs(X);
      const float absY = std::abs(Y);
      const bool bResult = std::abs(absX - absY) <= ENGINE_FLOAT_EPSILON;
      return bResult;
   }

   float LerpFloat(const float src, const float dst, const float factor)
   {
      assert(factor <= 1 && factor >= 0);
      return (dst - src) * factor + src;
   }
}