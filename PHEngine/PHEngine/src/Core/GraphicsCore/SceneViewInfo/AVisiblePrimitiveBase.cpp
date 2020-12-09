#include "AVisiblePrimitiveBase.h"

namespace Graphics
{

   AVisiblePrimitiveBase::AVisiblePrimitiveBase()
      : mBoundingBox()
   {
   }

   const BoundingBox& AVisiblePrimitiveBase::GetBoundingBox() const
   {
      return mBoundingBox;
   }
}