#include "EngineMath.h"

namespace EngineMath
{
   bool CompareFloats(const float X, const float Y)
   {
      const float absX = std::abs(X);
      const float absY = std::abs(Y);
      const bool bResult = std::abs(absX - absY) <= FLT_EPSILON;
      return bResult;
   }
}