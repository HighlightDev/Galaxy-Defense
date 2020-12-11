#include "AVisiblePrimitiveBase.h"

namespace Graphics
{

   AVisiblePrimitiveBase::AVisiblePrimitiveBase()
      : mBoundingBox()
   {
   }

   BoundingBox AVisiblePrimitiveBase::GetTransformedBoundingBox() const
   {
      return mBoundingBox;
   }

   void AVisiblePrimitiveBase::SetTransformedBoundingBox(const BoundingBox& boundingBox) {

      mBoundingBox = boundingBox;
   }

}